
#include "../../../../src/infrastructure/database/DatabaseManager.h"
#include "../../../../src/infrastructure/database/ObjectTypeRepository.h"
#include "../../../../src/infrastructure/database/ObjectRepository.h"
#include "../../../../src/infrastructure/database/ObjectRelationRepository.h"
#include "../../../../src/infrastructure/database/PropertyDefinitionRepository.h"
#include "../../../../src/infrastructure/database/PropertyValueRepository.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QSqlQuery>
#include <QStandardPaths>

namespace
{
    int failedTests = 0;
    int passedTests = 0;

    void check(const QString &name, bool passed)
    {
        if (passed)
        {
            ++passedTests;
            qInfo().noquote() << "PASS:" << name;
        }
        else
        {
            ++failedTests;
            qCritical().noquote() << "FAIL:" << name;
        }
    }

    bool hasTable(const QSqlDatabase &database, const QString &tableName)
    {
        QSqlQuery query(database);

        query.prepare(QStringLiteral(
            "SELECT 1 FROM sqlite_master "
            "WHERE type = 'table' AND name = :name"
        ));

        query.bindValue(QStringLiteral(":name"), tableName);

        return query.exec() && query.next();
    }

    bool hasIndex(const QSqlDatabase &database, const QString &indexName)
    {
        QSqlQuery query(database);

        query.prepare(QStringLiteral(
            "SELECT 1 FROM sqlite_master "
            "WHERE type = 'index' AND name = :name"
        ));

        query.bindValue(QStringLiteral(":name"), indexName);

        return query.exec() && query.next();
    }

    bool queryFails(const QSqlDatabase &database, const QString &sql)
    {
        QSqlQuery query(database);
        return !query.exec(sql);
    }

}

int main(int argc, char *argv[])
{
    QCoreApplication application(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("CreatorOS"));
    QCoreApplication::setApplicationName(QStringLiteral("CreatorOSDatabaseTests"));
    QStandardPaths::setTestModeEnabled(true);

    const QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(appDataPath);

    const QString databasePath = QDir(appDataPath).filePath(QStringLiteral("creatoros.sqlite"));
    QFile::remove(databasePath);

    {
        DatabaseManager databaseManager;
        QString errorMessage;

        const bool initialized = databaseManager.initialize(&errorMessage);

        check(QStringLiteral("Database initialization succeeds"), initialized);

        if (!initialized)
        {
            qCritical().noquote() << "Initialization error:" << errorMessage;
            return 1;
        }

        const QSqlDatabase database = databaseManager.database();
        const QStringList expectedTables = {
            QStringLiteral("object_types"),
            QStringLiteral("objects"),
            QStringLiteral("object_relations"),
            QStringLiteral("property_definitions"),
            QStringLiteral("property_text_settings"),
            QStringLiteral("property_entity_reference_settings"),
            QStringLiteral("property_options"),
            QStringLiteral("property_values")
        };

        for (const QString &table : expectedTables)
            check(QStringLiteral("Table exists: ") + table, hasTable(database, table));

        const QStringList expectedIndexes = {
            QStringLiteral("ux_object_types_name"),
            QStringLiteral("ux_objects_active_name"),
            QStringLiteral("ix_objects_object_type_id"),
            QStringLiteral("ix_objects_deleted_at"),
            QStringLiteral("ix_object_relations_child_object_id"),
            QStringLiteral("ux_property_definitions_common_name"),
            QStringLiteral("ux_property_definitions_object_type_name"),
            QStringLiteral("ix_property_definitions_object_type_id"),
            QStringLiteral("ix_property_values_property_id")
        };

        for (const QString &index : expectedIndexes)
            check(QStringLiteral("Index exists: ") + index, hasIndex(database, index));

        QSqlQuery versionQuery(database);

        const bool versionRead = versionQuery.exec(QStringLiteral("PRAGMA user_version")) && versionQuery.next();
        check(QStringLiteral("Database schema version is 1"), versionRead && versionQuery.value(0).toInt() == 1);
        QSqlQuery foreignKeysQuery(database);

        const bool foreignKeysEnabled = foreignKeysQuery.exec(QStringLiteral("PRAGMA foreign_keys")) && foreignKeysQuery.next() && foreignKeysQuery.value(0).toInt() == 1;

        check(QStringLiteral("SQLite foreign keys are enabled"), foreignKeysEnabled);

    // Проверка ObjectTypeRepository.
        try
        {
            ObjectTypeRepository objectTypeRepository(databaseManager);
            ObjectType objectType;
            objectType.uuid = "test-object-type-uuid";
            objectType.name = "Test Object Type";
            objectType.description = "Test description";
            objectType.createdAt = "2026-11-24T10:00:00";
            objectType.updatedAt = "2026-11-24T10:00:00";

            const std::int64_t objectTypeId = objectTypeRepository.create(objectType);
            check(QStringLiteral("ObjectType is created"), objectTypeId > 0);

            const auto loadedObjectType = objectTypeRepository.findById(objectTypeId);
            check(QStringLiteral("ObjectType is read correctly"), loadedObjectType.has_value()
                && loadedObjectType->uuid == objectType.uuid && loadedObjectType->name == objectType.name
                && loadedObjectType->description == objectType.description && loadedObjectType->createdAt == objectType.createdAt
            );

            check(QStringLiteral("ObjectType name existence is detected"), objectTypeRepository.existsByName(objectType.name));
            check(QStringLiteral("ObjectType name exclusion works"), !objectTypeRepository.existsByName(objectType.name, objectTypeId));

            if (loadedObjectType.has_value())
            {
                ObjectType updatedObjectType = loadedObjectType.value();
                updatedObjectType.name = "Updated Test Object Type";
                updatedObjectType.description = "Updated description";
                updatedObjectType.updatedAt = "2026-11-24T11:00:00";

                const bool updated = objectTypeRepository.update(updatedObjectType);
                const auto reloadedObjectType = objectTypeRepository.findById(objectTypeId);

                check(
                    QStringLiteral("ObjectType is updated correctly"), updated
                    && reloadedObjectType.has_value() && reloadedObjectType->name == updatedObjectType.name
                    && reloadedObjectType->description == updatedObjectType.description && reloadedObjectType->updatedAt == updatedObjectType.updatedAt
                );
            }

            check(QStringLiteral("ObjectType findAll returns created record"), !objectTypeRepository.findAll().empty());
        }
        catch (const std::exception &exception)
        {
            check(QStringLiteral("ObjectTypeRepository operations"), false);
            qCritical() << exception.what();
        }

    // Проверка ObjectRepository.
        try
        {
            ObjectTypeRepository objectTypeRepository(databaseManager);
            ObjectRepository objectRepository(databaseManager);

            ObjectType objectType;
            objectType.uuid = "object-test-type-uuid";
            objectType.name = "Object Test Type";
            objectType.createdAt = "2026-11-24T10:00:00";
            objectType.updatedAt = "2026-11-24T10:00:00";

            const std::int64_t objectTypeId = objectTypeRepository.create(objectType);

            Object object;
            object.uuid = "test-object-uuid";
            object.objectTypeId = objectTypeId;
            object.name = "Test Object";
            object.description = "Object description";
            object.deadlineAt = "2026-12-31";
            object.progress = 42.5;
            object.createdAt = "2026-11-24T10:00:00";
            object.updatedAt = "2026-11-24T10:00:00";

            const std::int64_t objectId = objectRepository.create(object);

            check(QStringLiteral("Object is created"), objectId > 0);

            const auto loadedObject = objectRepository.findById(objectId);

            check(QStringLiteral("Object is read correctly"), loadedObject.has_value() && loadedObject->uuid == object.uuid
                && loadedObject->objectTypeId == object.objectTypeId && loadedObject->name == object.name
                && loadedObject->description == object.description && loadedObject->deadlineAt == object.deadlineAt
                && loadedObject->progress == object.progress && loadedObject->createdAt == object.createdAt
            );

            check(QStringLiteral("Object name existence is detected"), objectRepository.existsActiveByName(object.name));
            check(QStringLiteral("Object is found by ObjectType"), !objectRepository.findActiveByObjectTypeId(objectTypeId).empty());
            check(QStringLiteral("Object is included in active objects"), !objectRepository.findAllActive().empty());

            if (loadedObject.has_value())
            {
                Object updatedObject = loadedObject.value();
                updatedObject.name = "Updated Test Object";
                updatedObject.progress = 75.0;
                updatedObject.updatedAt = "2026-11-24T11:00:00";

                const bool updated = objectRepository.update(updatedObject);
                const auto reloadedObject = objectRepository.findById(objectId);

                check(QStringLiteral("Object is updated correctly"), updated
                    && reloadedObject.has_value() && reloadedObject->name == updatedObject.name
                    && reloadedObject->progress == updatedObject.progress && reloadedObject->updatedAt == updatedObject.updatedAt
                );
            }
        }
        catch (const std::exception &exception)
        {
            check(QStringLiteral("ObjectRepository operations"), false);
            qCritical() << exception.what();
        }

    // Проверка ObjectRelationRepository.
        try
        {
            ObjectTypeRepository objectTypeRepository(databaseManager);
            ObjectRepository objectRepository(databaseManager);
            ObjectRelationRepository relationRepository(databaseManager);

            ObjectType objectType;
            objectType.uuid = "relation-test-type-uuid";
            objectType.name = "Relation Test Type";
            objectType.createdAt = "2026-11-24T10:00:00";
            objectType.updatedAt = "2026-11-24T10:00:00";

            const std::int64_t objectTypeId = objectTypeRepository.create(objectType);

            Object parentObject;
            parentObject.uuid = "relation-test-parent-uuid";
            parentObject.objectTypeId = objectTypeId;
            parentObject.name = "Relation Test Parent";
            parentObject.createdAt = "2026-11-24T10:00:00";
            parentObject.updatedAt = "2026-11-24T10:00:00";

            const std::int64_t parentObjectId = objectRepository.create(parentObject);

            Object childObject;
            childObject.uuid = "relation-test-child-uuid";
            childObject.objectTypeId = objectTypeId;
            childObject.name = "Relation Test Child";
            childObject.createdAt = "2026-11-24T10:00:00";
            childObject.updatedAt = "2026-11-24T10:00:00";

            const std::int64_t childObjectId = objectRepository.create(childObject);

            ObjectRelation relation;
            relation.uuid = "test-object-relation-uuid";
            relation.parentObjectId = parentObjectId;
            relation.childObjectId = childObjectId;
            relation.role = ObjectRelationRole::Main;
            relation.createdAt = "2026-11-24T10:00:00";
            relation.updatedAt = "2026-11-24T10:00:00";

            const std::int64_t relationId = relationRepository.create(relation);
            check(QStringLiteral("ObjectRelation is created"), relationId > 0);
            const auto loadedRelation = relationRepository.findById(relationId);
            check(QStringLiteral("ObjectRelation is read correctly"),
                loadedRelation.has_value() && loadedRelation->uuid == relation.uuid
                && loadedRelation->parentObjectId == parentObjectId && loadedRelation->childObjectId == childObjectId
                && loadedRelation->role == relation.role && loadedRelation->createdAt == relation.createdAt
            );

            const auto parentRelations = relationRepository.findByParentObjectId(parentObjectId);
            const auto childRelations = relationRepository.findByChildObjectId(childObjectId);

            check(QStringLiteral("ObjectRelation is found by parent"), parentRelations.size() == 1 && parentRelations.front().id == relationId);
            check(QStringLiteral("ObjectRelation is found by child"), childRelations.size() == 1 && childRelations.front().id == relationId);

            if (loadedRelation.has_value())
            {
                ObjectRelation updatedRelation = loadedRelation.value();
                updatedRelation.role = ObjectRelationRole::Support;
                updatedRelation.updatedAt = "2026-11-24T11:00:00";

                const bool updated = relationRepository.update(updatedRelation);
                const auto reloadedRelation = relationRepository.findById(relationId);

                check(QStringLiteral("ObjectRelation is updated correctly"), updated && reloadedRelation.has_value()
                    && reloadedRelation->role == updatedRelation.role && reloadedRelation->updatedAt == updatedRelation.updatedAt
                );
            }

            const bool removed = relationRepository.remove(relationId);
            const auto removedRelation = relationRepository.findById(relationId);

            check(QStringLiteral("ObjectRelation is removed"), removed && !removedRelation.has_value());
        }
        catch (const std::exception &exception)
        {
            check(QStringLiteral("ObjectRelationRepository operations"), false);
            qCritical() << exception.what();
        }

    // Проверка PropertyDefinitionRepository и PropertyValueRepository.
        try
        {
            ObjectTypeRepository objectTypeRepository(databaseManager);
            ObjectRepository objectRepository(databaseManager);
            ObjectRelationRepository relationRepository(databaseManager);
            PropertyDefinitionRepository propertyRepository(databaseManager);
            PropertyValueRepository valueRepository(databaseManager);

        // Создаём ObjectType и Object для проверок свойств.
            ObjectType objectType;
            objectType.uuid = "property-test-type-uuid";
            objectType.name = "Property Test Type";
            objectType.createdAt = "2026-11-24T10:00:00";
            objectType.updatedAt = "2026-11-24T10:00:00";

            const std::int64_t objectTypeId = objectTypeRepository.create(objectType);

            Object object;
            object.uuid = "property-test-object-uuid";
            object.objectTypeId = objectTypeId;
            object.name = "Property Test Object";
            object.createdAt = "2026-11-24T10:00:00";
            object.updatedAt = "2026-11-24T10:00:00";

            const std::int64_t objectId = objectRepository.create(object);

            check(QStringLiteral("Property test prerequisites are created"), objectTypeId > 0 && objectId > 0);

        // Общее текстовое свойство.
            PropertyDefinition commonDefinition;
            commonDefinition.uuid = "property-common-text-uuid";
            commonDefinition.scope = PropertyDefinitionScope::Common;
            commonDefinition.name = "Common Text Property";
            commonDefinition.description = "Common property description";
            commonDefinition.valueType = PropertyValueType::Text;
            commonDefinition.isMultiple = true;
            commonDefinition.isRequired = true;
            commonDefinition.defaultValue = "Default text";
            commonDefinition.isEnabled = true;
            commonDefinition.showOnCard = true;
            commonDefinition.createdAt = "2026-11-24T10:00:00";
            commonDefinition.updatedAt = "2026-11-24T10:00:00";

            const std::int64_t commonPropertyId = propertyRepository.create(commonDefinition);

            check(QStringLiteral("Common PropertyDefinition is created"), commonPropertyId > 0);

            const auto loadedCommonDefinition = propertyRepository.findById(commonPropertyId);

            check(QStringLiteral("Common PropertyDefinition is read correctly"),
                loadedCommonDefinition.has_value() && loadedCommonDefinition->uuid == commonDefinition.uuid
                && loadedCommonDefinition->scope == commonDefinition.scope && !loadedCommonDefinition->objectTypeId.has_value()
                && loadedCommonDefinition->name == commonDefinition.name && loadedCommonDefinition->description == commonDefinition.description
                && loadedCommonDefinition->valueType == commonDefinition.valueType && loadedCommonDefinition->isMultiple == commonDefinition.isMultiple
                && loadedCommonDefinition->isRequired == commonDefinition.isRequired && loadedCommonDefinition->defaultValue == commonDefinition.defaultValue
                && loadedCommonDefinition->isEnabled == commonDefinition.isEnabled && loadedCommonDefinition->showOnCard == commonDefinition.showOnCard
            );

            check(QStringLiteral("Common PropertyDefinition is found by scope"), !propertyRepository.findCommon().empty());

        // Свойство конкретного ObjectType.
            PropertyDefinition typeDefinition;
            typeDefinition.uuid = "property-type-text-uuid";
            typeDefinition.scope = PropertyDefinitionScope::ObjectType;
            typeDefinition.objectTypeId = objectTypeId;
            typeDefinition.name = "Type Text Property";
            typeDefinition.valueType = PropertyValueType::Text;
            typeDefinition.createdAt = "2026-11-24T10:00:00";
            typeDefinition.updatedAt = "2026-11-24T10:00:00";

            const std::int64_t typePropertyId = propertyRepository.create(typeDefinition);
            const auto typeDefinitions = propertyRepository.findByObjectTypeId(objectTypeId);

            check(QStringLiteral("ObjectType PropertyDefinition is created and found"), typePropertyId > 0
                && !typeDefinitions.empty() && typeDefinitions.front().objectTypeId == objectTypeId
            );

        // Настройки текстового свойства.
            PropertyTextSettings textSettings;
            textSettings.propertyId = commonPropertyId;
            textSettings.minLength = 2;
            textSettings.maxLength = 100;

            check(QStringLiteral("Text settings are saved"), propertyRepository.saveTextSettings(textSettings));

            const auto loadedTextSettings = propertyRepository.findTextSettings(commonPropertyId);

            check(QStringLiteral("Text settings are read correctly"), loadedTextSettings.has_value()
                && loadedTextSettings->minLength == textSettings.minLength && loadedTextSettings->maxLength == textSettings.maxLength
            );

        // Проверяем обновление настроек через повторное сохранение.
            textSettings.maxLength = 200;

            const bool textSettingsUpdated = propertyRepository.saveTextSettings(textSettings);
            const auto updatedTextSettings = propertyRepository.findTextSettings(commonPropertyId);

            check(QStringLiteral("Text settings are updated"), textSettingsUpdated
                && updatedTextSettings.has_value() && updatedTextSettings->maxLength == textSettings.maxLength
            );

        // Настройки ссылки на другую сущность.
            PropertyDefinition referenceDefinition;
            referenceDefinition.uuid = "property-reference-uuid";
            referenceDefinition.scope = PropertyDefinitionScope::Common;
            referenceDefinition.name = "Entity Reference Property";
            referenceDefinition.valueType = PropertyValueType::EntityReference;
            referenceDefinition.createdAt = "2026-11-24T10:00:00";
            referenceDefinition.updatedAt = "2026-11-24T10:00:00";

            const std::int64_t referencePropertyId = propertyRepository.create(referenceDefinition);

            PropertyEntityReferenceSettings referenceSettings;
            referenceSettings.propertyId = referencePropertyId;
            referenceSettings.targetEntityType = "Object";

            check(QStringLiteral("Entity reference settings are saved"), propertyRepository.saveEntityReferenceSettings(referenceSettings));

            const auto loadedReferenceSettings = propertyRepository.findEntityReferenceSettings(referencePropertyId);

            check(QStringLiteral("Entity reference settings are read correctly"),
                loadedReferenceSettings.has_value() && loadedReferenceSettings->targetEntityType == referenceSettings.targetEntityType
            );

            referenceSettings.targetEntityType = "Task";

            const bool referenceSettingsUpdated = propertyRepository.saveEntityReferenceSettings(referenceSettings);
            const auto updatedReferenceSettings = propertyRepository.findEntityReferenceSettings(referencePropertyId);

            check(QStringLiteral("Entity reference settings are updated"), referenceSettingsUpdated
                && updatedReferenceSettings.has_value() && updatedReferenceSettings->targetEntityType == "Task"
            );

        // Варианты значений.
            PropertyOption option;
            option.propertyId = commonPropertyId;
            option.value = "First option";
            option.normalizedValue = "first option";

            const std::int64_t optionId = propertyRepository.createOption(option);

            check(QStringLiteral("PropertyOption is created"), optionId > 0);

            auto options = propertyRepository.findOptions(commonPropertyId);

            check(QStringLiteral("PropertyOptions are read correctly"), options.size() == 1 && options.front().id == optionId
                && options.front().value == option.value && options.front().normalizedValue == option.normalizedValue
            );

            if (!options.empty())
            {
                PropertyOption updatedOption = options.front();
                updatedOption.value = "Updated option";
                updatedOption.normalizedValue = "updated option";

                const bool optionUpdated = propertyRepository.updateOption(updatedOption);
                options = propertyRepository.findOptions(commonPropertyId);

                check(QStringLiteral("PropertyOption is updated"), optionUpdated && options.size() == 1
                    && options.front().value == updatedOption.value && options.front().normalizedValue == updatedOption.normalizedValue
                );
            }

        // Значения свойства объекта.
            PropertyValue propertyValue;
            propertyValue.objectId = objectId;
            propertyValue.propertyId = commonPropertyId;
            propertyValue.value = "Initial property value";

            check(QStringLiteral("PropertyValue is saved"), valueRepository.save(propertyValue));

            const auto loadedPropertyValue = valueRepository.findByObjectAndProperty(objectId, commonPropertyId);

            check(
                QStringLiteral("PropertyValue is read correctly"),
                loadedPropertyValue.has_value()
                && loadedPropertyValue->objectId == objectId
                && loadedPropertyValue->propertyId == commonPropertyId
                && loadedPropertyValue->value == propertyValue.value
            );

            const auto objectValues = valueRepository.findByObjectId(objectId);
            check(QStringLiteral("PropertyValues are found by Object"), !objectValues.empty());

            propertyValue.value = "Updated property value";

            const bool propertyValueUpdated = valueRepository.save(propertyValue);
            const auto updatedPropertyValue = valueRepository.findByObjectAndProperty(objectId, commonPropertyId);

            check(QStringLiteral("PropertyValue is updated"), propertyValueUpdated && updatedPropertyValue.has_value() && updatedPropertyValue->value == propertyValue.value);

        // Изменение типа свойства должно удалять настройки,
        // которые больше не соответствуют новому типу.
            PropertyDefinition updatedDefinition = loadedCommonDefinition.value();
            updatedDefinition.valueType = PropertyValueType::Number;
            updatedDefinition.updatedAt = "2026-11-24T11:00:00";

            const bool definitionUpdated = propertyRepository.update(updatedDefinition);
            const auto reloadedDefinition = propertyRepository.findById(commonPropertyId);
            const auto removedTextSettings = propertyRepository.findTextSettings(commonPropertyId);

            check(
                QStringLiteral("PropertyDefinition is updated"),
                definitionUpdated
                && reloadedDefinition.has_value()
                && reloadedDefinition->valueType == PropertyValueType::Number
                && reloadedDefinition->updatedAt == updatedDefinition.updatedAt
            );

            check(QStringLiteral("Incompatible text settings are removed after type change"), !removedTextSettings.has_value());

        // Проверяем удаление настроек вручную.
            check(QStringLiteral("Entity reference settings are removed"),
                propertyRepository.removeEntityReferenceSettings(referencePropertyId) && !propertyRepository.findEntityReferenceSettings(referencePropertyId).has_value()
            );

        // Проверяем удаление вариантов.
            check(
                QStringLiteral("PropertyOption is removed"),
                !propertyRepository.findOptions(commonPropertyId).empty()
                && propertyRepository.removeOption(optionId)
                && propertyRepository.findOptions(commonPropertyId).empty()
            );

        // Проверяем удаление значения.
            check(QStringLiteral("PropertyValue is removed"),
                valueRepository.remove(objectId, commonPropertyId) && !valueRepository.findByObjectAndProperty(objectId, commonPropertyId).has_value()
            );

        // Проверяем удаление определения свойства.
            check(QStringLiteral("PropertyDefinition is removed"), propertyRepository.remove(commonPropertyId) && !propertyRepository.findById(commonPropertyId).has_value());
            check(QStringLiteral("Removing PropertyDefinition removes its settings"), !propertyRepository.findTextSettings(commonPropertyId).has_value());

        // Проверяем каскадное удаление настроек, вариантов и значений.
            PropertyDefinition cascadeDefinition;
            cascadeDefinition.uuid = "property-cascade-test-uuid";
            cascadeDefinition.scope = PropertyDefinitionScope::Common;
            cascadeDefinition.name = "Cascade Test Property";
            cascadeDefinition.valueType = PropertyValueType::Text;
            cascadeDefinition.createdAt = "2026-11-24T10:00:00";
            cascadeDefinition.updatedAt = "2026-11-24T10:00:00";

            const std::int64_t cascadePropertyId = propertyRepository.create(cascadeDefinition);

            PropertyTextSettings cascadeSettings;
            cascadeSettings.propertyId = cascadePropertyId;
            cascadeSettings.minLength = 1;
            cascadeSettings.maxLength = 50;

            propertyRepository.saveTextSettings(cascadeSettings);

            PropertyOption cascadeOption;
            cascadeOption.propertyId = cascadePropertyId;
            cascadeOption.value = "Cascade option";
            cascadeOption.normalizedValue = "cascade option";

            const std::int64_t cascadeOptionId = propertyRepository.createOption(cascadeOption);

            PropertyValue cascadeValue;
            cascadeValue.objectId = objectId;
            cascadeValue.propertyId = cascadePropertyId;
            cascadeValue.value = "Cascade value";

            valueRepository.save(cascadeValue);

            const bool cascadeRemoved = propertyRepository.remove(cascadePropertyId);

            check(
                QStringLiteral("PropertyDefinition removal cascades to dependent records"),
                cascadeRemoved
                && !propertyRepository.findTextSettings(cascadePropertyId).has_value()
                && propertyRepository.findOptions(cascadePropertyId).empty()
                && !valueRepository.findByObjectAndProperty(objectId, cascadePropertyId).has_value()
            );

            Q_UNUSED(cascadeOptionId);

        // Проверка удаления Object с каскадным удалением отношений и значений.
            Object childObject;
            childObject.uuid = "cascade-child-object-uuid";
            childObject.objectTypeId = objectTypeId;
            childObject.name = "Cascade Child Object";
            childObject.createdAt = "2026-11-24T10:00:00";
            childObject.updatedAt = "2026-11-24T10:00:00";

            const std::int64_t childObjectId = objectRepository.create(childObject);

            ObjectRelation cascadeRelation;
            cascadeRelation.uuid = "cascade-object-relation-uuid";
            cascadeRelation.parentObjectId = objectId;
            cascadeRelation.childObjectId = childObjectId;
            cascadeRelation.role = ObjectRelationRole::Addition;
            cascadeRelation.createdAt = "2026-11-24T10:00:00";
            cascadeRelation.updatedAt = "2026-11-24T10:00:00";

            const std::int64_t cascadeRelationId = relationRepository.create(cascadeRelation);

            PropertyDefinition objectCascadeDefinition;
            objectCascadeDefinition.uuid = "object-cascade-property-uuid";
            objectCascadeDefinition.scope = PropertyDefinitionScope::Common;
            objectCascadeDefinition.name = "Object Cascade Property";
            objectCascadeDefinition.valueType = PropertyValueType::Text;
            objectCascadeDefinition.createdAt = "2026-11-24T10:00:00";
            objectCascadeDefinition.updatedAt = "2026-11-24T10:00:00";

            const std::int64_t objectCascadePropertyId = propertyRepository.create(objectCascadeDefinition);

            PropertyValue objectCascadeValue;
            objectCascadeValue.objectId = childObjectId;
            objectCascadeValue.propertyId = objectCascadePropertyId;
            objectCascadeValue.value = "Will be deleted";

            valueRepository.save(objectCascadeValue);

            const bool childRemoved = objectRepository.removePermanently(childObjectId);

            check(
                QStringLiteral("Object removal cascades to relations and property values"),
                childRemoved
                && !relationRepository.findById(cascadeRelationId).has_value()
                && !valueRepository.findByObjectAndProperty(childObjectId, objectCascadePropertyId).has_value()
            );

            propertyRepository.remove(objectCascadePropertyId);

        // Проверка транзакционного отката.
            PropertyDefinition rollbackDefinition;
            rollbackDefinition.uuid = "property-rollback-test-uuid";
            rollbackDefinition.scope = PropertyDefinitionScope::Common;
            rollbackDefinition.name = "Rollback Test Property";
            rollbackDefinition.valueType = PropertyValueType::Text;
            rollbackDefinition.createdAt = "2026-11-24T10:00:00";
            rollbackDefinition.updatedAt = "2026-11-24T10:00:00";

            const std::int64_t rollbackPropertyId = propertyRepository.create(rollbackDefinition);

            PropertyTextSettings rollbackSettings;
            rollbackSettings.propertyId = rollbackPropertyId;
            rollbackSettings.minLength = 3;
            rollbackSettings.maxLength = 30;

            propertyRepository.saveTextSettings(rollbackSettings);

            PropertyDefinition invalidUpdate = rollbackDefinition;
            invalidUpdate.id = rollbackPropertyId;
            invalidUpdate.scope = PropertyDefinitionScope::ObjectType;
            invalidUpdate.objectTypeId = -999;
            invalidUpdate.valueType = PropertyValueType::Number;
            invalidUpdate.updatedAt = "2026-11-24T12:00:00";

            bool updateFailed = false;

            try
            {
                propertyRepository.update(invalidUpdate);
            }
            catch (const std::exception &)
            {
                updateFailed = true;
            }

            const auto definitionAfterRollback = propertyRepository.findById(rollbackPropertyId);
            const auto settingsAfterRollback = propertyRepository.findTextSettings(rollbackPropertyId);

            check(
                QStringLiteral("Failed PropertyDefinition update is rolled back"),
                updateFailed
                && definitionAfterRollback.has_value()
                && definitionAfterRollback->valueType == PropertyValueType::Text
                && definitionAfterRollback->scope == PropertyDefinitionScope::Common
                && !definitionAfterRollback->objectTypeId.has_value()
                && settingsAfterRollback.has_value()
                && settingsAfterRollback->minLength == rollbackSettings.minLength
                && settingsAfterRollback->maxLength == rollbackSettings.maxLength
            );

            propertyRepository.remove(rollbackPropertyId);
        }
        catch (const std::exception &exception)
        {
            check(QStringLiteral("Property repositories operations"), false);
            qCritical() << exception.what();
        }

    // Проверка ограничений целостности SQLite.
        try
        {
            ObjectTypeRepository objectTypeRepository(databaseManager);
            ObjectRepository objectRepository(databaseManager);
            ObjectRelationRepository relationRepository(databaseManager);
            PropertyDefinitionRepository propertyRepository(databaseManager);

        // UUID ObjectType должен быть уникальным.
            ObjectType duplicateUuidType;
            duplicateUuidType.uuid = "test-object-type-uuid";
            duplicateUuidType.name = "Duplicate UUID Type";
            duplicateUuidType.createdAt = "2026-11-24T10:00:00";
            duplicateUuidType.updatedAt = "2026-11-24T10:00:00";

            bool duplicateUuidRejected = false;

            try
            {
                objectTypeRepository.create(duplicateUuidType);
            }
            catch (const std::exception &)
            {
                duplicateUuidRejected = true;
            }

            check(QStringLiteral("Duplicate ObjectType UUID is rejected"), duplicateUuidRejected);

        // Имена ObjectType уникальны без учёта регистра.
            ObjectType duplicateNameType;
            duplicateNameType.uuid = "duplicate-object-type-name-uuid";
            duplicateNameType.name = "updated test object type";
            duplicateNameType.createdAt = "2026-11-24T10:00:00";
            duplicateNameType.updatedAt = "2026-11-24T10:00:00";

            bool duplicateNameRejected = false;

            try
            {
                objectTypeRepository.create(duplicateNameType);
            }
            catch (const std::exception &)
            {
                duplicateNameRejected = true;
            }

            check(QStringLiteral("Duplicate ObjectType name is rejected case-insensitively"), duplicateNameRejected);

        // Прогресс Object не может выходить за диапазон 0–100.
            ObjectType constraintType;
            constraintType.uuid = "constraint-test-type-uuid";
            constraintType.name = "Constraint Test Type";
            constraintType.createdAt = "2026-11-24T10:00:00";
            constraintType.updatedAt = "2026-11-24T10:00:00";

            const std::int64_t constraintTypeId = objectTypeRepository.create(constraintType);

            Object invalidProgressObject;
            invalidProgressObject.uuid = "invalid-progress-object-uuid";
            invalidProgressObject.objectTypeId = constraintTypeId;
            invalidProgressObject.name = "Invalid Progress Object";
            invalidProgressObject.progress = 101.0;
            invalidProgressObject.createdAt = "2026-11-24T10:00:00";
            invalidProgressObject.updatedAt = "2026-11-24T10:00:00";

            bool invalidProgressRejected = false;

            try
            {
                objectRepository.create(invalidProgressObject);
            }
            catch (const std::exception &)
            {
                invalidProgressRejected = true;
            }

            check(QStringLiteral("Object progress outside 0–100 is rejected"), invalidProgressRejected);

        // ObjectRelation не может ссылаться сам на себя.
            Object relationObject;
            relationObject.uuid = "constraint-relation-object-uuid";
            relationObject.objectTypeId = constraintTypeId;
            relationObject.name = "Constraint Relation Object";
            relationObject.createdAt = "2026-11-24T10:00:00";
            relationObject.updatedAt = "2026-11-24T10:00:00";

            const std::int64_t relationObjectId = objectRepository.create(relationObject);

            ObjectRelation selfRelation;
            selfRelation.uuid = "constraint-self-relation-uuid";
            selfRelation.parentObjectId = relationObjectId;
            selfRelation.childObjectId = relationObjectId;
            selfRelation.role = ObjectRelationRole::Main;
            selfRelation.createdAt = "2026-11-24T10:00:00";
            selfRelation.updatedAt = "2026-11-24T10:00:00";

            bool selfRelationRejected = false;

            try
            {
                relationRepository.create(selfRelation);
            }
            catch (const std::exception &)
            {
                selfRelationRejected = true;
            }

            check(QStringLiteral("ObjectRelation cannot reference itself"), selfRelationRejected);

        // Нельзя создать отношение с несуществующим Object.
            ObjectRelation invalidForeignKeyRelation;
            invalidForeignKeyRelation.uuid = "invalid-foreign-key-relation-uuid";
            invalidForeignKeyRelation.parentObjectId = -100;
            invalidForeignKeyRelation.childObjectId = relationObjectId;
            invalidForeignKeyRelation.role = ObjectRelationRole::Main;
            invalidForeignKeyRelation.createdAt = "2026-11-24T10:00:00";
            invalidForeignKeyRelation.updatedAt = "2026-11-24T10:00:00";

            bool foreignKeyRejected = false;

            try
            {
                relationRepository.create(invalidForeignKeyRelation);
            }
            catch (const std::exception &)
            {
                foreignKeyRejected = true;
            }

            check(QStringLiteral("Invalid ObjectRelation foreign key is rejected"), foreignKeyRejected);

            // Проверка CHECK-ограничений таблицы property_definitions.
            check(QStringLiteral("Invalid PropertyDefinition value type is rejected"), queryFails(database,
                    QStringLiteral("INSERT INTO property_definitions (uuid, scope, object_type_id, name, value_type, created_at, updated_at) "
                        "VALUES ('invalid-value-type-uuid', 'common', NULL, 'Invalid Value Type', 'invalid', '2026-11-24', '2026-11-24')"))
            );

            check(QStringLiteral("Invalid PropertyDefinition scope is rejected"), queryFails(database,
                    QStringLiteral("INSERT INTO property_definitions (uuid, scope, object_type_id, name, value_type, created_at, updated_at) "
                        "VALUES ('invalid-scope-uuid', 'invalid', NULL, 'Invalid Scope', 'text', '2026-11-24', '2026-11-24')"))
            );

            check(QStringLiteral("Common PropertyDefinition cannot have ObjectType ID"), queryFails(database,
                    QStringLiteral("INSERT INTO property_definitions (uuid, scope, object_type_id, name, value_type, created_at, updated_at) "
                        "VALUES ('invalid-common-type-uuid', 'common', 1, 'Invalid Common Property', 'text', '2026-11-24', '2026-11-24')"))
            );

            check(QStringLiteral("Text settings reject negative minimum length"), queryFails(database,
                    QStringLiteral("INSERT INTO property_text_settings (property_id, min_length, max_length) VALUES (-100, -1, 10)"))
                );

            Q_UNUSED(propertyRepository);
        }
        catch (const std::exception &exception)
        {
            check(QStringLiteral("Database integrity constraints"), false);
            qCritical() << exception.what();
        }

    // Проверка AUTOINCREMENT: удалённые ID не используются повторно.
        try
        {
            ObjectTypeRepository objectTypeRepository(databaseManager);
            ObjectRepository objectRepository(databaseManager);
            ObjectRelationRepository relationRepository(databaseManager);
            PropertyDefinitionRepository propertyRepository(databaseManager);

        // ObjectType.
            ObjectType firstType;
            firstType.uuid = "autoincrement-type-first-uuid";
            firstType.name = "Autoincrement Type First";
            firstType.createdAt = "2026-11-24T10:00:00";
            firstType.updatedAt = "2026-11-24T10:00:00";

            const std::int64_t firstTypeId = objectTypeRepository.create(firstType);
            const bool firstTypeRemoved = objectTypeRepository.remove(firstTypeId);

            ObjectType secondType;
            secondType.uuid = "autoincrement-type-second-uuid";
            secondType.name = "Autoincrement Type Second";
            secondType.createdAt = "2026-11-24T10:00:00";
            secondType.updatedAt = "2026-11-24T10:00:00";

            const std::int64_t secondTypeId = objectTypeRepository.create(secondType);
            check(QStringLiteral("ObjectType ID is not reused"), firstTypeRemoved && secondTypeId > firstTypeId);

        // Object.
            Object firstObject;
            firstObject.uuid = "autoincrement-object-first-uuid";
            firstObject.objectTypeId = secondTypeId;
            firstObject.name = "Autoincrement Object First";
            firstObject.createdAt = "2026-11-24T10:00:00";
            firstObject.updatedAt = "2026-11-24T10:00:00";

            const std::int64_t firstObjectId = objectRepository.create(firstObject);
            const bool firstObjectRemoved = objectRepository.removePermanently(firstObjectId);

            Object secondObject;
            secondObject.uuid = "autoincrement-object-second-uuid";
            secondObject.objectTypeId = secondTypeId;
            secondObject.name = "Autoincrement Object Second";
            secondObject.createdAt = "2026-11-24T10:00:00";
            secondObject.updatedAt = "2026-11-24T10:00:00";

            const std::int64_t secondObjectId = objectRepository.create(secondObject);
            check(QStringLiteral("Object ID is not reused"), firstObjectRemoved && secondObjectId > firstObjectId);

        // ObjectRelation.
            ObjectRelation firstRelation;
            firstRelation.uuid = "autoincrement-relation-first-uuid";
            firstRelation.parentObjectId = secondObjectId;
            firstRelation.childObjectId = relationObjectId;
            firstRelation.role = ObjectRelationRole::Main;
            firstRelation.createdAt = "2026-11-24T10:00:00";
            firstRelation.updatedAt = "2026-11-24T10:00:00";

            const std::int64_t firstRelationId = relationRepository.create(firstRelation);
            const bool firstRelationRemoved = relationRepository.remove(firstRelationId);

            ObjectRelation secondRelation;
            secondRelation.uuid = "autoincrement-relation-second-uuid";
            secondRelation.parentObjectId = secondObjectId;
            secondRelation.childObjectId = relationObjectId;
            secondRelation.role = ObjectRelationRole::Addition;
            secondRelation.createdAt = "2026-11-24T10:00:00";
            secondRelation.updatedAt = "2026-11-24T10:00:00";

            const std::int64_t secondRelationId = relationRepository.create(secondRelation);
            check(QStringLiteral("ObjectRelation ID is not reused"), firstRelationRemoved && secondRelationId > firstRelationId);

        // PropertyDefinition.
            PropertyDefinition firstDefinition;
            firstDefinition.uuid = "autoincrement-property-first-uuid";
            firstDefinition.scope = PropertyDefinitionScope::Common;
            firstDefinition.name = "Autoincrement Property First";
            firstDefinition.valueType = PropertyValueType::Text;
            firstDefinition.createdAt = "2026-11-24T10:00:00";
            firstDefinition.updatedAt = "2026-11-24T10:00:00";

            const std::int64_t firstDefinitionId = propertyRepository.create(firstDefinition);
            const bool firstDefinitionRemoved = propertyRepository.remove(firstDefinitionId);

            PropertyDefinition secondDefinition;
            secondDefinition.uuid = "autoincrement-property-second-uuid";
            secondDefinition.scope = PropertyDefinitionScope::Common;
            secondDefinition.name = "Autoincrement Property Second";
            secondDefinition.valueType = PropertyValueType::Text;
            secondDefinition.createdAt = "2026-11-24T10:00:00";
            secondDefinition.updatedAt = "2026-11-24T10:00:00";

            const std::int64_t secondDefinitionId = propertyRepository.create(secondDefinition);
            check(QStringLiteral("PropertyDefinition ID is not reused"), firstDefinitionRemoved && secondDefinitionId > firstDefinitionId);

        // PropertyOption.
            PropertyOption firstOption;
            firstOption.propertyId = secondDefinitionId;
            firstOption.value = "First";
            firstOption.normalizedValue = "first";

            const std::int64_t firstOptionId = propertyRepository.createOption(firstOption);
            const bool firstOptionRemoved = propertyRepository.removeOption(firstOptionId);

            PropertyOption secondOption;
            secondOption.propertyId = secondDefinitionId;
            secondOption.value = "Second";
            secondOption.normalizedValue = "second";

            const std::int64_t secondOptionId = propertyRepository.createOption(secondOption);
            check(QStringLiteral("PropertyOption ID is not reused"), firstOptionRemoved && secondOptionId > firstOptionId);
        }
        catch (const std::exception &exception)
        {
            check(QStringLiteral("AUTOINCREMENT checks"), false);
            qCritical() << exception.what();
        }

        qInfo() << "Passed:" << passedTests;
        qInfo() << "Failed:" << failedTests;
    }

    QFile::remove(databasePath);

    return failedTests == 0 ? 0 : 1;
}
