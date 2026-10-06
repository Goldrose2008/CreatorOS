#include "DatabaseManager.h"

#include <QDir>
#include <QFile>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <stdexcept>

namespace
{
    bool applyMigration(QSqlDatabase &database, int version, const QString &resourcePath, QString *errorMessage)
    {
        QFile migrationFile(resourcePath);

        if (!migrationFile.open(QIODevice::ReadOnly))
        {
            if (errorMessage != nullptr)
            {
                *errorMessage = QStringLiteral("Database migration was not found: ") + resourcePath;
            }

            return false;
        }

        const QString sql = QString::fromUtf8(migrationFile.readAll());

        if (!database.transaction())
        {
            if (errorMessage != nullptr)
            {
                *errorMessage = database.lastError().text();
            }

            return false;
        }

        QSqlQuery schemaQuery(database);

        if (!schemaQuery.exec(sql))
        {
            database.rollback();

            if (errorMessage != nullptr)
            {
                *errorMessage = schemaQuery.lastError().text();
            }

            return false;
        }

        QSqlQuery versionUpdate(database);

        if (!versionUpdate.exec(QStringLiteral("PRAGMA user_version = %1").arg(version)))
        {
            database.rollback();

            if (errorMessage != nullptr)
            {
                *errorMessage = versionUpdate.lastError().text();
            }

            return false;
        }

        if (!database.commit())
        {
            if (errorMessage != nullptr)
            {
                *errorMessage = database.lastError().text();
            }

            return false;
        }

        return true;
    }
}

DatabaseManager::DatabaseManager() : connectionName_(QStringLiteral("CreatorOS.Main"))
{
}

DatabaseManager::~DatabaseManager()
{
    if (!database_.isValid()){return;}

    const QString connectionName = database_.connectionName();
    database_.close();
    database_ = QSqlDatabase();
    QSqlDatabase::removeDatabase(connectionName);
}

bool DatabaseManager::initialize(QString *errorMessage)
{
    if (database_.isOpen()){return true;}

    const QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    if (appDataPath.isEmpty())
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = QStringLiteral("Application data directory is unavailable.");
        }
        return false;
    }

    QDir directory;

    if (!directory.mkpath(appDataPath))
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = QStringLiteral("Failed to create application data directory.");
        }
        return false;
    }

    databasePath_ = QDir(appDataPath).filePath(QStringLiteral("creatoros.sqlite"));
    database_ = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connectionName_);
    database_.setDatabaseName(databasePath_);

    if (!database_.open())
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = database_.lastError().text();
        }
        return false;
    }

    QSqlQuery foreignKeysQuery(database_);

    if (!foreignKeysQuery.exec(QStringLiteral("PRAGMA foreign_keys = ON")))
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = foreignKeysQuery.lastError().text();
        }
        return false;
    }
    return applySchema(errorMessage);
}

QSqlDatabase DatabaseManager::database() const
{
    return database_;
}

QString DatabaseManager::databasePath() const
{
    return databasePath_;
}

bool DatabaseManager::applySchema(QString *errorMessage)
{
    QSqlQuery versionQuery(database_);

    if (!versionQuery.exec(QStringLiteral("PRAGMA user_version")))
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = versionQuery.lastError().text();
        }

        return false;
    }

    if (!versionQuery.next())
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = QStringLiteral("Failed to read database schema version.");
        }

        return false;
    }

    int currentVersion = versionQuery.value(0).toInt();

    struct Migration
    {
        int version;
        QString resourcePath;
    };

    const Migration migrations[] =
    {
        {
            1,
            QStringLiteral(":/database/migrations/001_initial.sql")
        },
        {
            2,
            QStringLiteral(":/database/migrations/002_content_and_content_types.sql")
        }
    };

    for (const Migration &migration : migrations)
    {
        if (currentVersion >= migration.version)
        {
            continue;
        }

        if (!applyMigration(database_, migration.version, migration.resourcePath, errorMessage))
        {
            return false;
        }

        currentVersion = migration.version;
    }

    return true;
}