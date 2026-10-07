#include "ContentRepository.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <stdexcept>

#include "DatabaseManager.h"

namespace
{
    Content readContent(const QSqlQuery &query)
    {
        Content content;
        content.id = query.value("id").toLongLong();
        content.projectId = query.value("project_id").toLongLong();
        content.contentTypeId = query.value("content_type_id").toLongLong();

        const auto role = contentRoleFromString(query.value("content_role").toString().toStdString());

        if (!role.has_value())
        {
            throw std::runtime_error("Database contains invalid Content role.");
        }

        content.role = role.value();
        content.name = query.value("name").toString().toStdString();
        content.description = query.value("description").toString().toStdString();
        content.priority = query.value("priority").toInt();
        content.productionDeadlineAt = query.value("production_deadline_at").toString().toStdString();

        const auto status = contentStatusFromString(query.value("status").toString().toStdString());

        if (!status.has_value())
        {
            throw std::runtime_error("Database contains invalid Content status.");
        }

        content.status = status.value();
        content.progress = query.value("progress").toInt();
        content.createdAt = query.value("created_at").toString().toStdString();
        content.updatedAt = query.value("updated_at").toString().toStdString();

        return content;
    }

    QString selectColumns()
    {
        return QStringLiteral(
            "id, "
            "project_id, "
            "content_type_id, "
            "content_role, "
            "name, "
            "description, "
            "priority, "
            "production_deadline_at, "
            "status, "
            "progress, "
            "created_at, "
            "updated_at "
        );
    }
}

ContentRepository::ContentRepository(const DatabaseManager &databaseManager)
    : database_(databaseManager.database())
{
}

std::vector<Content> ContentRepository::findByProjectId(std::int64_t projectId) const
{
    QSqlQuery query(database_);

    query.prepare(
        QStringLiteral("SELECT ") + selectColumns() +
        QStringLiteral(
            "FROM contents "
            "WHERE project_id = :project_id "
            "ORDER BY priority DESC, id ASC"
        )
    );

    query.bindValue(QStringLiteral(":project_id"), QVariant::fromValue(projectId));

    if (!query.exec())
    {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    std::vector<Content> contents;

    while (query.next())
    {
        contents.push_back(readContent(query));
    }

    return contents;
}

std::optional<Content> ContentRepository::findById(std::int64_t id) const
{
    QSqlQuery query(database_);

    query.prepare(
        QStringLiteral("SELECT ") + selectColumns() +
        QStringLiteral(
            "FROM contents "
            "WHERE id = :id"
        )
    );

    query.bindValue(QStringLiteral(":id"), QVariant::fromValue(id));

    if (!query.exec())
    {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    if (!query.next()){return std::nullopt;}

    return readContent(query);
}

std::int64_t ContentRepository::create(const Content &content)
{
    QSqlQuery query(database_);

    query.prepare(
        QStringLiteral(
            "INSERT INTO contents "
            "(project_id, content_type_id, content_role, name, "
            "description, priority, status, progress) "
            "VALUES "
            "(:project_id, :content_type_id, :content_role, :name, "
            ":description, :priority, :status, :progress)"
        )
    );

    query.bindValue(QStringLiteral(":project_id"), QVariant::fromValue(content.projectId));
    query.bindValue(QStringLiteral(":content_type_id"), QVariant::fromValue(content.contentTypeId));
    query.bindValue(QStringLiteral(":content_role"), QString::fromStdString(contentRoleToString(content.role)));
    query.bindValue(QStringLiteral(":name"), QString::fromUtf8(content.name.c_str()));
    query.bindValue(QStringLiteral(":description"), QString::fromUtf8(content.description.c_str()));
    query.bindValue(QStringLiteral(":priority"), content.priority);
    query.bindValue(QStringLiteral(":status"), QString::fromStdString(contentStatusToString(content.status)));
    query.bindValue(QStringLiteral(":progress"), content.progress);

    if (!query.exec())
    {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    return query.lastInsertId().toLongLong();
}

bool ContentRepository::update(const Content &content)
{
    QSqlQuery query(database_);

    query.prepare(
        QStringLiteral(
            "UPDATE contents "
            "SET "
            "content_type_id = :content_type_id, "
            "name = :name, "
            "description = :description, "
            "priority = :priority, "
            "updated_at = CURRENT_TIMESTAMP "
            "WHERE id = :id"
        )
    );

    query.bindValue(QStringLiteral(":content_type_id"), QVariant::fromValue(content.contentTypeId));
    query.bindValue(QStringLiteral(":name"), QString::fromUtf8(content.name.c_str()));
    query.bindValue(QStringLiteral(":description"), QString::fromUtf8(content.description.c_str()));
    query.bindValue(QStringLiteral(":priority"), content.priority);
    query.bindValue(QStringLiteral(":id"), QVariant::fromValue(content.id));

    if (!query.exec())
    {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    return query.numRowsAffected() > 0;
}

bool ContentRepository::remove(std::int64_t id)
{
    QSqlQuery query(database_);

    query.prepare(
        QStringLiteral(
            "DELETE FROM contents "
            "WHERE id = :id"
        )
    );

    query.bindValue(QStringLiteral(":id"), QVariant::fromValue(id));

    if (!query.exec())
    {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    return query.numRowsAffected() > 0;
}