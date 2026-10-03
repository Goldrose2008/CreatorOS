#include "DatabaseManager.h"

#include <QDir>
#include <QFile>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

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

    const int currentVersion = versionQuery.value(0).toInt();

    if (currentVersion >= 1){return true;}

    QFile migrationFile(QStringLiteral(":/database/migrations/001_initial.sql"));

    if (!migrationFile.open(QIODevice::ReadOnly))
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = QStringLiteral("Initial database migration was not found.");
        }
        return false;
    }

    const QString sql = QString::fromUtf8(migrationFile.readAll());

    if (!database_.transaction())
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = database_.lastError().text();
        }
        return false;
    }

    QSqlQuery schemaQuery(database_);

    if (!schemaQuery.exec(sql))
    {
        database_.rollback();
        if (errorMessage != nullptr)
        {
            *errorMessage = schemaQuery.lastError().text();
        }
        return false;
    }

    QSqlQuery versionUpdate(database_);

    if (!versionUpdate.exec(QStringLiteral("PRAGMA user_version = 1")))
    {
        database_.rollback();
        if (errorMessage != nullptr)
        {
            *errorMessage = versionUpdate.lastError().text();
        }
        return false;
    }

    if (!database_.commit())
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = database_.lastError().text();
        }
        return false;
    }

    return true;
}