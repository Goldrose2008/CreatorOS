#include "PropertyDefinitionRepository.h"

#include "DatabaseManager.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <stdexcept>

namespace
{
    PropertyDefinition readPropertyDefinition(const QSqlQuery &query)
    {
        PropertyDefinition definition;

        definition.id = query.value("id").toLongLong();
        definition.uuid = query.value("uuid").toString().toStdString();

        const std::string scopeValue = query.value("scope").toString().toStdString();
        const auto scope = propertyDefinitionScopeFromDatabaseValue(scopeValue);

        if (!scope.has_value())
            throw std::runtime_error("Unknown PropertyDefinition scope in database.");

        definition.scope = scope.value();

        if (!query.value("object_type_id").isNull())
            definition.objectTypeId = query.value("object_type_id").toLongLong();

        definition.name = query.value("name").toString().toStdString();

        if (!query.value("description").isNull())
            definition.description = query.value("description").toString().toStdString();

        const std::string valueTypeValue = query.value("value_type").toString().toStdString();
        const auto valueType = propertyValueTypeFromDatabaseValue(valueTypeValue);

        if (!valueType.has_value())
            throw std::runtime_error("Unknown PropertyValueType in database.");

        definition.valueType = valueType.value();

        definition.isMultiple = query.value("is_multiple").toBool();
        definition.isRequired = query.value("is_required").toBool();

        if (!query.value("default_value").isNull())
            definition.defaultValue = query.value("default_value").toString().toStdString();

        definition.isEnabled = query.value("is_enabled").toBool();
        definition.showOnCard = query.value("show_on_card").toBool();

        definition.createdAt = query.value("created_at").toString().toStdString();
        definition.updatedAt = query.value("updated_at").toString().toStdString();

        return definition;
    }

    QString selectColumns()
    {
        return QStringLiteral(
            "id, "
            "uuid, "
            "scope, "
            "object_type_id, "
            "name, "
            "description, "
            "value_type, "
            "is_multiple, "
            "is_required, "
            "default_value, "
            "is_enabled, "
            "show_on_card, "
            "created_at, "
            "updated_at "
        );
    }

    void bindDefinitionFields(
        QSqlQuery &query,
        const PropertyDefinition &definition
    )
    {
        const std::string scopeValue =
            propertyDefinitionScopeToDatabaseValue(definition.scope);

        const std::string valueTypeValue =
            propertyValueTypeToDatabaseValue(definition.valueType);

        query.bindValue(
            QStringLiteral(":scope"),
            QString::fromUtf8(scopeValue.c_str())
        );

        if (definition.objectTypeId.has_value())
        {
            query.bindValue(
                QStringLiteral(":object_type_id"),
                QVariant::fromValue(definition.objectTypeId.value())
            );
        }
        else
        {
            query.bindValue(QStringLiteral(":object_type_id"), QVariant());
        }

        query.bindValue(
            QStringLiteral(":name"),
            QString::fromUtf8(definition.name.c_str())
        );

        if (definition.description.has_value())
        {
            query.bindValue(
                QStringLiteral(":description"),
                QString::fromUtf8(definition.description->c_str())
            );
        }
        else
        {
            query.bindValue(QStringLiteral(":description"), QVariant());
        }

        query.bindValue(
            QStringLiteral(":value_type"),
            QString::fromUtf8(valueTypeValue.c_str())
        );

        query.bindValue(
            QStringLiteral(":is_multiple"),
            definition.isMultiple
        );

        query.bindValue(
            QStringLiteral(":is_required"),
            definition.isRequired
        );

        if (definition.defaultValue.has_value())
        {
            query.bindValue(
                QStringLiteral(":default_value"),
                QString::fromUtf8(definition.defaultValue->c_str())
            );
        }
        else
        {
            query.bindValue(QStringLiteral(":default_value"), QVariant());
        }

        query.bindValue(
            QStringLiteral(":is_enabled"),
            definition.isEnabled
        );

        query.bindValue(
            QStringLiteral(":show_on_card"),
            definition.showOnCard
        );
    }
}

PropertyDefinitionRepository::PropertyDefinitionRepository(const DatabaseManager &databaseManager) : database_(databaseManager.database())
{
}

std::vector<PropertyDefinition> PropertyDefinitionRepository::findCommon() const
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral("SELECT ") + selectColumns() + QStringLiteral("FROM property_definitions WHERE scope = 'common' ORDER BY name COLLATE NOCASE ASC, id ASC"));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    std::vector<PropertyDefinition> definitions;

    while (query.next())
        definitions.push_back(readPropertyDefinition(query));

    return definitions;
}

std::vector<PropertyDefinition> PropertyDefinitionRepository::findByObjectTypeId(std::int64_t objectTypeId) const
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral("SELECT ") + selectColumns() + QStringLiteral(
            "FROM property_definitions "
            "WHERE scope = 'object_type' "
            "AND object_type_id = :object_type_id "
            "ORDER BY name COLLATE NOCASE ASC, id ASC"
        )
    );

    query.bindValue(QStringLiteral(":object_type_id"), QVariant::fromValue(objectTypeId));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    std::vector<PropertyDefinition> definitions;

    while (query.next())
        definitions.push_back(readPropertyDefinition(query));

    return definitions;
}

std::optional<PropertyDefinition> PropertyDefinitionRepository::findById(std::int64_t id) const
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral("SELECT ") + selectColumns() + QStringLiteral("FROM property_definitions WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), QVariant::fromValue(id));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    if (!query.next())
        return std::nullopt;

    return readPropertyDefinition(query);
}

std::int64_t PropertyDefinitionRepository::create(const PropertyDefinition &definition)
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral(
        "INSERT INTO property_definitions ("
        "uuid, "
        "scope, "
        "object_type_id, "
        "name, "
        "description, "
        "value_type, "
        "is_multiple, "
        "is_required, "
        "default_value, "
        "is_enabled, "
        "show_on_card, "
        "created_at, "
        "updated_at"
        ") VALUES ("
        ":uuid, "
        ":scope, "
        ":object_type_id, "
        ":name, "
        ":description, "
        ":value_type, "
        ":is_multiple, "
        ":is_required, "
        ":default_value, "
        ":is_enabled, "
        ":show_on_card, "
        ":created_at, "
        ":updated_at"
        ")"
    ));

    query.bindValue(QStringLiteral(":uuid"), QString::fromUtf8(definition.uuid.c_str()));

    bindDefinitionFields(query, definition);

    query.bindValue(QStringLiteral(":created_at"), QString::fromUtf8(definition.createdAt.c_str()));
    query.bindValue(QStringLiteral(":updated_at"), QString::fromUtf8(definition.updatedAt.c_str()));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    return query.lastInsertId().toLongLong();
}

bool PropertyDefinitionRepository::update(const PropertyDefinition &definition)
{
    QSqlDatabase database = database_;

    if (!database.transaction())
        throw std::runtime_error(database.lastError().text().toStdString());

    QSqlQuery query(database);

    query.prepare(QStringLiteral(
        "UPDATE property_definitions SET "
        "scope = :scope, "
        "object_type_id = :object_type_id, "
        "name = :name, "
        "description = :description, "
        "value_type = :value_type, "
        "is_multiple = :is_multiple, "
        "is_required = :is_required, "
        "default_value = :default_value, "
        "is_enabled = :is_enabled, "
        "show_on_card = :show_on_card, "
        "updated_at = :updated_at "
        "WHERE id = :id"
    ));

    bindDefinitionFields(query, definition);

    query.bindValue(QStringLiteral(":updated_at"), QString::fromUtf8(definition.updatedAt.c_str()));
    query.bindValue(QStringLiteral(":id"), QVariant::fromValue(definition.id));

    if (!query.exec())
    {
        database.rollback();
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    if (query.numRowsAffected() <= 0)
    {
        if (!database.commit())
            throw std::runtime_error(database.lastError().text().toStdString());

        return false;
    }

    query.finish();

    const std::string valueTypeValue = propertyValueTypeToDatabaseValue(definition.valueType);
    QSqlQuery removeTextSettingsQuery(database);

    removeTextSettingsQuery.prepare(QStringLiteral(
        "DELETE FROM property_text_settings "
        "WHERE property_id = :property_id "
        "AND :value_type <> 'text'"
    ));

    removeTextSettingsQuery.bindValue(QStringLiteral(":property_id"), QVariant::fromValue(definition.id));
    removeTextSettingsQuery.bindValue(QStringLiteral(":value_type"), QString::fromUtf8(valueTypeValue.c_str()));

    if (!removeTextSettingsQuery.exec())
    {
        database.rollback();
        throw std::runtime_error(removeTextSettingsQuery.lastError().text().toStdString());
    }

    QSqlQuery removeEntityReferenceSettingsQuery(database);

    removeEntityReferenceSettingsQuery.prepare(QStringLiteral(
        "DELETE FROM property_entity_reference_settings "
        "WHERE property_id = :property_id "
        "AND :value_type <> 'entity_reference'"
    ));

    removeEntityReferenceSettingsQuery.bindValue(QStringLiteral(":property_id"), QVariant::fromValue(definition.id));
    removeEntityReferenceSettingsQuery.bindValue(QStringLiteral(":value_type"), QString::fromUtf8(valueTypeValue.c_str()));

    if (!removeEntityReferenceSettingsQuery.exec())
    {
        database.rollback();
        throw std::runtime_error(removeEntityReferenceSettingsQuery.lastError().text().toStdString());
    }

    if (!database.commit())
        throw std::runtime_error(database.lastError().text().toStdString());

    return true;
}

bool PropertyDefinitionRepository::remove(std::int64_t id)
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral("DELETE FROM property_definitions WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), QVariant::fromValue(id));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    return query.numRowsAffected() > 0;
}

std::optional<PropertyTextSettings> PropertyDefinitionRepository::findTextSettings(std::int64_t propertyId) const
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral("SELECT property_id, min_length, max_length FROM property_text_settings WHERE property_id = :property_id"));
    query.bindValue(QStringLiteral(":property_id"), QVariant::fromValue(propertyId));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    if (!query.next())
        return std::nullopt;

    PropertyTextSettings settings;

    settings.propertyId = query.value("property_id").toLongLong();

    if (!query.value("min_length").isNull())
        settings.minLength = query.value("min_length").toLongLong();

    if (!query.value("max_length").isNull())
        settings.maxLength = query.value("max_length").toLongLong();

    return settings;
}

bool PropertyDefinitionRepository::saveTextSettings(const PropertyTextSettings &settings)
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral(
        "INSERT INTO property_text_settings (property_id, min_length, max_length) "
        "VALUES (:property_id, :min_length, :max_length) "
        "ON CONFLICT(property_id) DO UPDATE SET "
        "min_length = excluded.min_length, "
        "max_length = excluded.max_length"
    ));

    query.bindValue(QStringLiteral(":property_id"), QVariant::fromValue(settings.propertyId));

    if (settings.minLength.has_value())
        query.bindValue(QStringLiteral(":min_length"), QVariant::fromValue(settings.minLength.value()));
    else
        query.bindValue(QStringLiteral(":min_length"), QVariant());

    if (settings.maxLength.has_value())
        query.bindValue(QStringLiteral(":max_length"), QVariant::fromValue(settings.maxLength.value()));
    else
        query.bindValue(QStringLiteral(":max_length"), QVariant());

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    return query.numRowsAffected() > 0;
}

bool PropertyDefinitionRepository::removeTextSettings(std::int64_t propertyId)
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral("DELETE FROM property_text_settings WHERE property_id = :property_id"));
    query.bindValue(QStringLiteral(":property_id"), QVariant::fromValue(propertyId));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    return query.numRowsAffected() > 0;
}

std::optional<PropertyEntityReferenceSettings>
PropertyDefinitionRepository::findEntityReferenceSettings(std::int64_t propertyId) const
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral("SELECT property_id, target_entity_type FROM property_entity_reference_settings WHERE property_id = :property_id"));
    query.bindValue(QStringLiteral(":property_id"), QVariant::fromValue(propertyId));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    if (!query.next())
        return std::nullopt;

    PropertyEntityReferenceSettings settings;

    settings.propertyId = query.value("property_id").toLongLong();
    settings.targetEntityType = query.value("target_entity_type").toString().toStdString();

    return settings;
}

bool PropertyDefinitionRepository::saveEntityReferenceSettings(const PropertyEntityReferenceSettings &settings)
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral(
        "INSERT INTO property_entity_reference_settings (property_id, target_entity_type) "
        "VALUES (:property_id, :target_entity_type) "
        "ON CONFLICT(property_id) DO UPDATE SET target_entity_type = excluded.target_entity_type"
    ));

    query.bindValue(QStringLiteral(":property_id"), QVariant::fromValue(settings.propertyId));
    query.bindValue(QStringLiteral(":target_entity_type"), QString::fromUtf8(settings.targetEntityType.c_str()));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    return query.numRowsAffected() > 0;
}

bool PropertyDefinitionRepository::removeEntityReferenceSettings(std::int64_t propertyId)
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral("DELETE FROM property_entity_reference_settings WHERE property_id = :property_id"));

    query.bindValue(QStringLiteral(":property_id"), QVariant::fromValue(propertyId));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    return query.numRowsAffected() > 0;
}

std::vector<PropertyOption> PropertyDefinitionRepository::findOptions(std::int64_t propertyId) const
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral(
        "SELECT id, property_id, value, normalized_value "
        "FROM property_options "
        "WHERE property_id = :property_id ORDER BY id ASC"
    ));

    query.bindValue(QStringLiteral(":property_id"), QVariant::fromValue(propertyId));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    std::vector<PropertyOption> options;

    while (query.next())
    {
        PropertyOption option;

        option.id = query.value("id").toLongLong();
        option.propertyId = query.value("property_id").toLongLong();
        option.value = query.value("value").toString().toStdString();
        option.normalizedValue = query.value("normalized_value").toString().toStdString();

        options.push_back(option);
    }

    return options;
}

std::int64_t PropertyDefinitionRepository::createOption(const PropertyOption &option)
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral(
        "INSERT INTO property_options (property_id, value, normalized_value) "
        "VALUES (:property_id, :value, :normalized_value)"
    ));
    query.bindValue(QStringLiteral(":property_id"), QVariant::fromValue(option.propertyId));
    query.bindValue(QStringLiteral(":value"), QString::fromUtf8(option.value.c_str()));
    query.bindValue(QStringLiteral(":normalized_value"), QString::fromUtf8(option.normalizedValue.c_str()));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    return query.lastInsertId().toLongLong();
}

bool PropertyDefinitionRepository::updateOption(const PropertyOption &option)
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral(
        "UPDATE property_options SET "
        "value = :value, "
        "normalized_value = :normalized_value "
        "WHERE id = :id"
    ));
    query.bindValue(QStringLiteral(":value"), QString::fromUtf8(option.value.c_str()));
    query.bindValue(QStringLiteral(":normalized_value"), QString::fromUtf8(option.normalizedValue.c_str()));
    query.bindValue(QStringLiteral(":id"), QVariant::fromValue(option.id));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    return query.numRowsAffected() > 0;
}

bool PropertyDefinitionRepository::removeOption(std::int64_t id)
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral("DELETE FROM property_options WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), QVariant::fromValue(id));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    return query.numRowsAffected() > 0;
}