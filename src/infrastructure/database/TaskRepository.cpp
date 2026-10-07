#include "TaskRepository.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <stdexcept>

#include "DatabaseManager.h"

namespace
{
    Task readTask(const QSqlQuery &query)
    {
        Task task;

        task.id = query.value("id").toLongLong();
        task.parentId = query.value("parent_id").toLongLong();

        const auto parentType = taskParentTypeFromString(query.value("parent_type").toString().toStdString());

        if (!parentType.has_value())
        {
            throw std::runtime_error("Database contains invalid Task parent type.");
        }

        task.parentType = parentType.value();

        const QVariant parentTaskIdValue = query.value("parent_task_id");

        if (parentTaskIdValue.isNull())
        {
            task.parentTaskId = std::nullopt;
        }
        else
        {
            task.parentTaskId = parentTaskIdValue.toLongLong();
        }

        task.name = query.value("name").toString().toStdString();
        task.description = query.value("description").toString().toStdString();
        task.deadlineAt = query.value("deadline_at").toString().toStdString();

        const auto status = taskStatusFromString(query.value("status").toString().toStdString());

        if (!status.has_value())
        {
            throw std::runtime_error("Database contains invalid Task status.");
        }

        task.status = status.value();
        task.priority = query.value("priority").toInt();
        task.createdAt = query.value("created_at").toString().toStdString();
        task.updatedAt = query.value("updated_at").toString().toStdString();

        return task;
    }

    QString selectColumns()
    {
        return QStringLiteral(
            "id, "
            "parent_id, "
            "parent_type, "
            "parent_task_id, "
            "name, "
            "description, "
            "deadline_at, "
            "status, "
            "priority, "
            "created_at, "
            "updated_at "
        );
    }
}

TaskRepository::TaskRepository(const DatabaseManager &databaseManager) : database_(databaseManager.database())
{
}

std::vector<Task> TaskRepository::findAll() const
{
    QSqlQuery query(database_);

    query.prepare(
        QStringLiteral("SELECT ") + selectColumns() +
        QStringLiteral(
            "FROM tasks "
            "ORDER BY priority DESC, id ASC"
        )
    );

    if (!query.exec())
    {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    std::vector<Task> tasks;

    while (query.next())
    {
        tasks.push_back(readTask(query));
    }

    return tasks;
}

std::vector<Task> TaskRepository::findByParent(TaskParentType parentType, std::int64_t parentId, const std::optional<std::int64_t> &parentTaskId) const
{
    QSqlQuery query(database_);

    QString sql =
        QStringLiteral("SELECT ") + selectColumns() +
        QStringLiteral(
            "FROM tasks "
            "WHERE parent_type = :parent_type "
            "AND parent_id = :parent_id "
        );

    if (parentTaskId.has_value())
    {
        sql += QStringLiteral("AND parent_task_id = :parent_task_id ");
    }
    else
    {
        sql += QStringLiteral("AND parent_task_id IS NULL ");
    }

    sql += QStringLiteral("ORDER BY priority DESC, id ASC");

    query.prepare(sql);
    query.bindValue(QStringLiteral(":parent_type"), QString::fromStdString(taskParentTypeToString(parentType)));
    query.bindValue(QStringLiteral(":parent_id"), QVariant::fromValue(parentId));

    if (parentTaskId.has_value())
    {
        query.bindValue(QStringLiteral(":parent_task_id"), QVariant::fromValue(parentTaskId.value()));
    }

    if (!query.exec())
    {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    std::vector<Task> tasks;

    while (query.next())
    {
        tasks.push_back(readTask(query));
    }

    return tasks;
}

std::optional<Task> TaskRepository::findById(std::int64_t id) const
{
    QSqlQuery query(database_);

    query.prepare(
        QStringLiteral("SELECT ") + selectColumns() +
        QStringLiteral(
            "FROM tasks "
            "WHERE id = :id"
        )
    );

    query.bindValue(QStringLiteral(":id"), QVariant::fromValue(id));

    if (!query.exec())
    {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    if (!query.next())
    {
        return std::nullopt;
    }

    return readTask(query);
}

std::int64_t TaskRepository::create(const Task &task)
{
    QSqlQuery query(database_);

    query.prepare(
        QStringLiteral(
            "INSERT INTO tasks "
            "(parent_id, parent_type, parent_task_id, "
            "name, description, deadline_at, status, priority) "
            "VALUES "
            "(:parent_id, :parent_type, :parent_task_id, "
            ":name, :description, :deadline_at, :status, :priority)"
        )
    );

    query.bindValue(QStringLiteral(":parent_id"), QVariant::fromValue(task.parentId));
    query.bindValue(QStringLiteral(":parent_type"), QString::fromStdString(taskParentTypeToString(task.parentType)));

    if (task.parentTaskId.has_value())
    {
        query.bindValue(QStringLiteral(":parent_task_id"), QVariant::fromValue(task.parentTaskId.value()));
    }
    else
    {
        query.bindValue(QStringLiteral(":parent_task_id"), QVariant());
    }

    query.bindValue(QStringLiteral(":name"), QString::fromUtf8(task.name.c_str()));
    query.bindValue(QStringLiteral(":description"), QString::fromUtf8(task.description.c_str()));
    query.bindValue(QStringLiteral(":deadline_at"), QString::fromUtf8(task.deadlineAt.c_str()));
    query.bindValue(QStringLiteral(":status"), QString::fromStdString(taskStatusToString(task.status)));
    query.bindValue(QStringLiteral(":priority"), task.priority);

    if (!query.exec())
    {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    return query.lastInsertId().toLongLong();
}

bool TaskRepository::update(const Task &task)
{
    QSqlQuery query(database_);

    query.prepare(
        QStringLiteral(
            "UPDATE tasks "
            "SET "
            "name = :name, "
            "description = :description, "
            "deadline_at = :deadline_at, "
            "updated_at = CURRENT_TIMESTAMP "
            "WHERE id = :id"
        )
    );

    query.bindValue(QStringLiteral(":name"), QString::fromUtf8(task.name.c_str()));

    query.bindValue(QStringLiteral(":description"), QString::fromUtf8(task.description.c_str()));

    query.bindValue(QStringLiteral(":deadline_at"), QString::fromUtf8(task.deadlineAt.c_str()));

    query.bindValue(QStringLiteral(":id"), QVariant::fromValue(task.id));

    if (!query.exec())
    {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    return query.numRowsAffected() > 0;
}

bool TaskRepository::remove(std::int64_t id)
{
    QSqlQuery query(database_);

    query.prepare(
        QStringLiteral(
            "DELETE FROM tasks "
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