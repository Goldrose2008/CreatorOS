
#include "../../../../src/infrastructure/database/DatabaseManager.h"
#include "../../../../src/infrastructure/database/ObjectTypeRepository.h"
#include "../../../../src/infrastructure/database/ObjectRepository.h"

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

        const bool foreignKeysEnabled =
            foreignKeysQuery.exec(QStringLiteral("PRAGMA foreign_keys"))
            && foreignKeysQuery.next()
            && foreignKeysQuery.value(0).toInt() == 1;

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
            check(
                QStringLiteral("ObjectType is read correctly"),
                loadedObjectType.has_value()
                && loadedObjectType->uuid == objectType.uuid
                && loadedObjectType->name == objectType.name
                && loadedObjectType->description == objectType.description
                && loadedObjectType->createdAt == objectType.createdAt
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
                    QStringLiteral("ObjectType is updated correctly"),
                    updated
                    && reloadedObjectType.has_value()
                    && reloadedObjectType->name == updatedObjectType.name
                    && reloadedObjectType->description == updatedObjectType.description
                    && reloadedObjectType->updatedAt == updatedObjectType.updatedAt
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

            check(
                QStringLiteral("Object is read correctly"),
                loadedObject.has_value()
                && loadedObject->uuid == object.uuid
                && loadedObject->objectTypeId == object.objectTypeId
                && loadedObject->name == object.name
                && loadedObject->description == object.description
                && loadedObject->deadlineAt == object.deadlineAt
                && loadedObject->progress == object.progress
                && loadedObject->createdAt == object.createdAt
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

                check(QStringLiteral("Object is updated correctly"),
                    updated
                    && reloadedObject.has_value()
                    && reloadedObject->name == updatedObject.name
                    && reloadedObject->progress == updatedObject.progress
                    && reloadedObject->updatedAt == updatedObject.updatedAt
                );
            }
        }
        catch (const std::exception &exception)
        {
            check(QStringLiteral("ObjectRepository operations"), false);
            qCritical() << exception.what();
        }

        qInfo() << "Passed:" << passedTests;
        qInfo() << "Failed:" << failedTests;
    }

    QFile::remove(databasePath);

    return failedTests == 0 ? 0 : 1;
}
