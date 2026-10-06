#include "ContentTypeRepository.h"

#include "DatabaseManager.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <stdexcept>

namespace
{
    ContentType readContentType(const QSqlQuery &query)
    {
        ContentType contentType;

        contentType.id = query.value("id").toLongLong();
        contentType.name = query.value("name").toString().toStdString();

        return contentType;
    }
}

ContentTypeRepository::ContentTypeRepository(const DatabaseManager &databaseManager) : database_(databaseManager.database())
{
}

std::vector<ContentType> ContentTypeRepository::findAll() const
{
    QSqlQuery query(database_);

    if (!query.exec(
        QStringLiteral(
            "SELECT "
            "id, "
            "name "
            "FROM content_types "
            "ORDER BY id ASC"
        )
    ))
    {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    std::vector<ContentType> contentTypes;

    while (query.next())
    {
        contentTypes.push_back(readContentType(query));
    }

    return contentTypes;
}

std::optional<ContentType> ContentTypeRepository::findById(std::int64_t id) const
{
    QSqlQuery query(database_);

    query.prepare(
        QStringLiteral(
            "SELECT "
            "id, "
            "name "
            "FROM content_types "
            "WHERE id = :id"
        )
    );

    query.bindValue(
        QStringLiteral(":id"),
        QVariant::fromValue(id)
    );

    if (!query.exec())
    {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    if (!query.next())
    {
        return std::nullopt;
    }

    return readContentType(query);
}