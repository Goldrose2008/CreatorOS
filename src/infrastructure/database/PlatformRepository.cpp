#include "PlatformRepository.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <stdexcept>

#include "DatabaseManager.h"

namespace
{
    Platform readPlatform(const QSqlQuery &query)
    {
        Platform platform;
        platform.id = query.value("id").toLongLong();
        platform.name = query.value("name").toString().toStdString();
        platform.websiteUrl = query.value("website_url").toString().toStdString();
        platform.createdAt = query.value("created_at").toString().toStdString();
        platform.updatedAt = query.value("updated_at").toString().toStdString();

        return platform;
    }

    QString selectColumns()
    {
        return QStringLiteral(
            "id, "
            "name, "
            "website_url, "
            "created_at, "
            "updated_at "
        );
    }
}

PlatformRepository::PlatformRepository(const DatabaseManager &databaseManager) : database_(databaseManager.database())
{
}

std::vector<Platform> PlatformRepository::findAll() const
{
    QSqlQuery query(database_);

    query.prepare(
        QStringLiteral("SELECT ") + selectColumns() +
        QStringLiteral("FROM platforms ORDER BY name COLLATE NOCASE ASC, id ASC")
    );

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    std::vector<Platform> platforms;

    while (query.next())
        platforms.push_back(readPlatform(query));

    return platforms;
}

std::optional<Platform> PlatformRepository::findById(std::int64_t id) const
{
    QSqlQuery query(database_);

    query.prepare(
        QStringLiteral("SELECT ") + selectColumns() +
        QStringLiteral("FROM platforms WHERE id = :id")
    );

    query.bindValue(QStringLiteral(":id"), QVariant::fromValue(id));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    if (!query.next())
        return std::nullopt;

    return readPlatform(query);
}

bool PlatformRepository::hasAccounts(std::int64_t id) const
{
    QSqlQuery query(database_);

    query.prepare(
        QStringLiteral(
            "SELECT 1 FROM accounts "
            "WHERE platform_id = :platform_id LIMIT 1"
        )
    );

    query.bindValue(QStringLiteral(":platform_id"), QVariant::fromValue(id));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    return query.next();
}

std::int64_t PlatformRepository::create(const Platform &platform)
{
    QSqlQuery query(database_);

    query.prepare(
        QStringLiteral(
            "INSERT INTO platforms (name, website_url) "
            "VALUES (:name, :website_url)"
        )
    );

    query.bindValue(QStringLiteral(":name"), QString::fromUtf8(platform.name.c_str()));
    query.bindValue(QStringLiteral(":website_url"), QString::fromUtf8(platform.websiteUrl.c_str()));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    return query.lastInsertId().toLongLong();
}

bool PlatformRepository::update(const Platform &platform)
{
    QSqlQuery query(database_);

    query.prepare(
        QStringLiteral(
            "UPDATE platforms "
            "SET name = :name, "
            "website_url = :website_url, "
            "updated_at = CURRENT_TIMESTAMP "
            "WHERE id = :id"
        )
    );

    query.bindValue(QStringLiteral(":name"), QString::fromUtf8(platform.name.c_str()));
    query.bindValue(QStringLiteral(":website_url"), QString::fromUtf8(platform.websiteUrl.c_str()));
    query.bindValue(QStringLiteral(":id"), QVariant::fromValue(platform.id));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    return query.numRowsAffected() > 0;
}

bool PlatformRepository::remove(std::int64_t id)
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral("DELETE FROM platforms WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), QVariant::fromValue(id));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    return query.numRowsAffected() > 0;
}