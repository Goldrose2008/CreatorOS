#include "ProjectRepository.h"

#include "DatabaseManager.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include <stdexcept>

namespace
{
    Project readProject(const QSqlQuery &query)
    {
        Project project;
        project.id = query.value("id").toLongLong();
        project.name = query.value("name").toString().toStdString();
        project.description = query.value("description").toString().toStdString();

        if (!query.value("owner_id").isNull())
        {
            project.ownerId = query.value("owner_id").toLongLong();
        }

        project.plannedReleaseAt = query.value("planned_release_at").toString().toStdString();
        const std::string statusValue = query.value("status").toString().toStdString();
        const auto status = projectStatusFromString(statusValue);

        if (!status.has_value())
        {
            throw std::runtime_error("Database contains invalid Project status.");
        }

        project.status = status.value();
        project.progress = query.value("progress").toInt();
        project.createdAt = query.value("created_at").toString().toStdString();
        project.updatedAt = query.value("updated_at").toString().toStdString();

        return project;
    }
}

ProjectRepository::ProjectRepository(const DatabaseManager &databaseManager)
    : database_(databaseManager.database())
{
}

std::vector<Project> ProjectRepository::findAll() const
{
    QSqlQuery query(database_);

    if (!query.exec(
        QStringLiteral(
            "SELECT "
            "id, "
            "name, "
            "description, "
            "owner_id, "
            "planned_release_at, "
            "status, "
            "progress, "
            "created_at, "
            "updated_at "
            "FROM projects "
            "ORDER BY planned_release_at ASC, id ASC"
        )
    ))
    {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    std::vector<Project> projects;

    while (query.next())
    {
        projects.push_back(readProject(query));
    }

    return projects;
}

std::optional<Project> ProjectRepository::findById(std::int64_t id) const
{
    QSqlQuery query(database_);

    query.prepare(
        QStringLiteral(
            "SELECT "
            "id, "
            "name, "
            "description, "
            "owner_id, "
            "planned_release_at, "
            "status, "
            "progress, "
            "created_at, "
            "updated_at "
            "FROM projects "
            "WHERE id = :id"
        )
    );

    query.bindValue(QStringLiteral(":id"), QVariant::fromValue(id));

    if (!query.exec())
    {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    if (!query.next()){return std::nullopt;}

    return readProject(query);
}

std::int64_t ProjectRepository::createWithMainContent(const Project &project, const Content &mainContent)
{
    QSqlDatabase database = database_;

    if (!database.transaction())
    {
        throw std::runtime_error(database.lastError().text().toStdString());
    }

    QSqlQuery projectQuery(database);

    projectQuery.prepare(
        QStringLiteral(
            "INSERT INTO projects "
            "(name, description, owner_id, "
            "planned_release_at, status, progress) "
            "VALUES "
            "(:name, :description, :owner_id, "
            ":planned_release_at, :status, :progress)"
        )
    );

    projectQuery.bindValue(
        QStringLiteral(":name"), 
        QString::fromUtf8(project.name.c_str())
    );

    projectQuery.bindValue(
        QStringLiteral(":description"), 
        QString::fromUtf8(project.description.c_str())
    );

    if (project.ownerId.has_value())
    {
        projectQuery.bindValue(
            QStringLiteral(":owner_id"), 
            QVariant::fromValue(project.ownerId.value()));
    }
    else
    {
        projectQuery.bindValue(
            QStringLiteral(":owner_id"), 
            QVariant());
    }

    projectQuery.bindValue(
        QStringLiteral(":planned_release_at"),
        QString::fromUtf8(project.plannedReleaseAt.c_str())
    );

    projectQuery.bindValue(
        QStringLiteral(":status"),
        QString::fromStdString(projectStatusToString(project.status))
    );

    projectQuery.bindValue(
        QStringLiteral(":progress"),
        project.progress
    );

    if (!projectQuery.exec())
    {
        database.rollback();
        throw std::runtime_error(projectQuery.lastError().text().toStdString());
    }

    const std::int64_t projectId = projectQuery.lastInsertId().toLongLong();

    QSqlQuery contentQuery(database);

    contentQuery.prepare(
        QStringLiteral(
            "INSERT INTO contents "
            "(project_id, content_type_id, content_role, name) "
            "VALUES "
            "(:project_id, :content_type_id, :content_role, :name)"
        )
    );

    contentQuery.bindValue(
        QStringLiteral(":project_id"),
        QVariant::fromValue(projectId)
    );

    contentQuery.bindValue(
        QStringLiteral(":content_type_id"),
        QVariant::fromValue(mainContent.contentTypeId)
    );

    contentQuery.bindValue(
        QStringLiteral(":content_role"),
        QString::fromStdString(contentRoleToString(ContentRole::Main))
    );

    contentQuery.bindValue(
        QStringLiteral(":name"),
        QString::fromUtf8(mainContent.name.c_str())
    );

    if (!contentQuery.exec())
    {
        database.rollback();
        throw std::runtime_error(contentQuery.lastError().text().toStdString());
    }

    if (!database.commit())
    {
        throw std::runtime_error(database.lastError().text().toStdString());
    }

    return projectId;
}

bool ProjectRepository::update(const Project &project)
{
    QSqlQuery query(database_);

    query.prepare(
        QStringLiteral(
            "UPDATE projects "
            "SET "
            "name = :name, "
            "description = :description, "
            "planned_release_at = :planned_release_at, "
            "updated_at = CURRENT_TIMESTAMP "
            "WHERE id = :id"
        )
    );

    query.bindValue(
        QStringLiteral(":name"),
        QString::fromUtf8(project.name.c_str())
    );

    query.bindValue(
        QStringLiteral(":description"),
        QString::fromUtf8(project.description.c_str())
    );

    query.bindValue(
        QStringLiteral(":planned_release_at"),
        QString::fromUtf8(project.plannedReleaseAt.c_str())
    );

    query.bindValue(
        QStringLiteral(":id"),
        QVariant::fromValue(project.id)
    );

    if (!query.exec())
    {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    return true;
}

bool ProjectRepository::remove(std::int64_t id)
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral("DELETE FROM projects " "WHERE id = :id"));

    query.bindValue(
        QStringLiteral(":id"),
        QVariant::fromValue(id)
    );

    if (!query.exec())
    {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    return query.numRowsAffected() > 0;
}