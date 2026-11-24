
#include "../../../../src/infrastructure/database/DatabaseManager.h"

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

    const QString appDataPath =
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    QDir().mkpath(appDataPath);

    const QString databasePath =
        QDir(appDataPath).filePath(QStringLiteral("creatoros.sqlite"));

    QFile::remove(databasePath);

    {
        DatabaseManager databaseManager;
        QString errorMessage;

        const bool initialized = databaseManager.initialize(&errorMessage);

        check(
            QStringLiteral("Database initialization succeeds"),
            initialized
        );

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
        {
            check(
                QStringLiteral("Table exists: ") + table,
                hasTable(database, table)
            );
        }

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
        {
            check(
                QStringLiteral("Index exists: ") + index,
                hasIndex(database, index)
            );
        }

        QSqlQuery versionQuery(database);

        const bool versionRead =
            versionQuery.exec(QStringLiteral("PRAGMA user_version"))
            && versionQuery.next();

        check(
            QStringLiteral("Database schema version is 1"),
            versionRead && versionQuery.value(0).toInt() == 1
        );

        QSqlQuery foreignKeysQuery(database);

        const bool foreignKeysEnabled =
            foreignKeysQuery.exec(QStringLiteral("PRAGMA foreign_keys"))
            && foreignKeysQuery.next()
            && foreignKeysQuery.value(0).toInt() == 1;

        check(
            QStringLiteral("SQLite foreign keys are enabled"),
            foreignKeysEnabled
        );

        qInfo() << "Passed:" << passedTests;
        qInfo() << "Failed:" << failedTests;
    }

    QFile::remove(databasePath);

    return failedTests == 0 ? 0 : 1;
}
