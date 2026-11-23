
#include "ObjectRepository.h"

#include "DatabaseManager.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include <stdexcept>

namespace
{
    Object readObject(const QSqlQuery &query)
    {
        Object object;

        object.id = query.value("id").toLongLong();
        object.uuid = query.value("uuid").toString().toStdString();

        if (!query.value("object_type_id").isNull())
            object.objectTypeId = query.value("object_type_id").toLongLong();

        object.name = query.value("name").toString().toStdString();

        if (!query.value("description").isNull())
            object.description = query.value("description").toString().toStdString();

        if (!query.value("deadline_at").isNull())
            object.deadlineAt = query.value("deadline_at").toString().toStdString();

        if (!query.value("progress").isNull())
            object.progress = query.value("progress").toDouble();

        object.createdAt = query.value("created_at").toString().toStdString();
        object.updatedAt = query.value("updated_at").toString().toStdString();

        if (!query.value("deleted_at").isNull())
            object.deletedAt = query.value("deleted_at").toString().toStdString();

        if (!query.value("former_object_type_uuid").isNull())
            object.formerObjectTypeUuid = query.value("former_object_type_uuid").toString().toStdString();

        if (!query.value("former_object_type_name").isNull())
            object.formerObjectTypeName = query.value("former_object_type_name").toString().toStdString();

        if (!query.value("deleted_timezone").isNull())
            object.deletedTimezone = query.value("deleted_timezone").toString().toStdString();

        return object;
    }

    QString selectColumns()
    {
        return QStringLiteral(
            "id, "
            "uuid, "
            "object_type_id, "
            "name, "
            "description, "
            "deadline_at, "
            "progress, "
            "created_at, "
            "updated_at, "
            "deleted_at, "
            "former_object_type_uuid, "
            "former_object_type_name, "
            "deleted_timezone "
        );
    }
}

ObjectRepository::ObjectRepository(const DatabaseManager &databaseManager) : database_(databaseManager.database())
{
}

std::vector<Object> ObjectRepository::findAllActive() const
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral("SELECT ") + selectColumns() + QStringLiteral("FROM objects WHERE deleted_at IS NULL ORDER BY name COLLATE NOCASE ASC, id ASC"));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    std::vector<Object> objects;

    while (query.next())
        objects.push_back(readObject(query));

    return objects;
}

std::vector<Object> ObjectRepository::findAllInTrash() const
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral("SELECT ") + selectColumns() + QStringLiteral("FROM objects WHERE deleted_at IS NOT NULL ORDER BY name COLLATE NOCASE ASC, id ASC"));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    std::vector<Object> objects;

    while (query.next())
        objects.push_back(readObject(query));

    return objects;
}

std::optional<Object> ObjectRepository::findById(std::int64_t id) const
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral("SELECT ") + selectColumns() + QStringLiteral("FROM objects WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), QVariant::fromValue(id));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    if (!query.next())
        return std::nullopt;

    return readObject(query);
}

std::vector<Object> ObjectRepository::findActiveByObjectTypeId(std::int64_t objectTypeId) const
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral("SELECT ") + selectColumns() + QStringLiteral(
            "FROM objects "
            "WHERE object_type_id = :object_type_id "
            "AND deleted_at IS NULL "
            "ORDER BY name COLLATE NOCASE ASC, id ASC"
        )
    );
    query.bindValue(QStringLiteral(":object_type_id"), QVariant::fromValue(objectTypeId));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    std::vector<Object> objects;

    while (query.next())
        objects.push_back(readObject(query));

    return objects;
}

bool ObjectRepository::existsActiveByName(const std::string &name, const std::optional<std::int64_t> &excludeId) const
{
    QSqlQuery query(database_);

    QString sql = QStringLiteral("SELECT 1 FROM objects WHERE name = :name COLLATE NOCASE AND deleted_at IS NULL");

    if (excludeId.has_value())
        sql += QStringLiteral(" AND id <> :exclude_id");

    sql += QStringLiteral(" LIMIT 1");

    query.prepare(sql);
    query.bindValue(QStringLiteral(":name"), QString::fromUtf8(name.c_str()));

    if (excludeId.has_value())
        query.bindValue(QStringLiteral(":exclude_id"), QVariant::fromValue(excludeId.value()));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    return query.next();
}

std::int64_t ObjectRepository::create(const Object &object)
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral(
        "INSERT INTO objects ("
        "uuid, "
        "object_type_id, "
        "name, "
        "description, "
        "deadline_at, "
        "progress, "
        "created_at, "
        "updated_at, "
        "deleted_at, "
        "former_object_type_uuid, "
        "former_object_type_name, "
        "deleted_timezone"
        ") VALUES ("
        ":uuid, "
        ":object_type_id, "
        ":name, "
        ":description, "
        ":deadline_at, "
        ":progress, "
        ":created_at, "
        ":updated_at, "
        ":deleted_at, "
        ":former_object_type_uuid, "
        ":former_object_type_name, "
        ":deleted_timezone"
        ")"
    ));

    query.bindValue(QStringLiteral(":uuid"), QString::fromUtf8(object.uuid.c_str()));

    if (object.objectTypeId.has_value())
        query.bindValue(QStringLiteral(":object_type_id"), QVariant::fromValue(object.objectTypeId.value()));
    else
        query.bindValue(QStringLiteral(":object_type_id"), QVariant());

    query.bindValue(QStringLiteral(":name"), QString::fromUtf8(object.name.c_str()));

    if (object.description.has_value())
        query.bindValue(QStringLiteral(":description"), QString::fromUtf8(object.description->c_str()));
    else
        query.bindValue(QStringLiteral(":description"), QVariant());

    if (object.deadlineAt.has_value())
        query.bindValue(QStringLiteral(":deadline_at"), QString::fromUtf8(object.deadlineAt->c_str()));
    else
        query.bindValue(QStringLiteral(":deadline_at"), QVariant());

    if (object.progress.has_value())
        query.bindValue(QStringLiteral(":progress"), object.progress.value());
    else
        query.bindValue(QStringLiteral(":progress"), QVariant());

    query.bindValue(QStringLiteral(":created_at"), QString::fromUtf8(object.createdAt.c_str()));
    query.bindValue(QStringLiteral(":updated_at"), QString::fromUtf8(object.updatedAt.c_str()));

    if (object.deletedAt.has_value())
        query.bindValue(QStringLiteral(":deleted_at"), QString::fromUtf8(object.deletedAt->c_str()));
    else
        query.bindValue(QStringLiteral(":deleted_at"), QVariant());

    if (object.formerObjectTypeUuid.has_value())
        query.bindValue(QStringLiteral(":former_object_type_uuid"), QString::fromUtf8(object.formerObjectTypeUuid->c_str()));
    else
        query.bindValue(QStringLiteral(":former_object_type_uuid"), QVariant());

    if (object.formerObjectTypeName.has_value())
        query.bindValue(QStringLiteral(":former_object_type_name"), QString::fromUtf8(object.formerObjectTypeName->c_str()));
    else
        query.bindValue(QStringLiteral(":former_object_type_name"), QVariant());

    if (object.deletedTimezone.has_value())
        query.bindValue(QStringLiteral(":deleted_timezone"), QString::fromUtf8(object.deletedTimezone->c_str()));
    else
        query.bindValue(QStringLiteral(":deleted_timezone"), QVariant());

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    return query.lastInsertId().toLongLong();
}

bool ObjectRepository::update(const Object &object)
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral(
        "UPDATE objects SET "
        "object_type_id = :object_type_id, "
        "name = :name, "
        "description = :description, "
        "deadline_at = :deadline_at, "
        "progress = :progress, "
        "updated_at = :updated_at, "
        "deleted_at = :deleted_at, "
        "former_object_type_uuid = :former_object_type_uuid, "
        "former_object_type_name = :former_object_type_name, "
        "deleted_timezone = :deleted_timezone "
        "WHERE id = :id"
    ));

    if (object.objectTypeId.has_value())
        query.bindValue(QStringLiteral(":object_type_id"), QVariant::fromValue(object.objectTypeId.value()));
    else
        query.bindValue(QStringLiteral(":object_type_id"), QVariant());

    query.bindValue(QStringLiteral(":name"), QString::fromUtf8(object.name.c_str()));

    if (object.description.has_value())
        query.bindValue(QStringLiteral(":description"), QString::fromUtf8(object.description->c_str()));
    else
        query.bindValue(QStringLiteral(":description"), QVariant());

    if (object.deadlineAt.has_value())
        query.bindValue(QStringLiteral(":deadline_at"), QString::fromUtf8(object.deadlineAt->c_str()));
    else
        query.bindValue(QStringLiteral(":deadline_at"), QVariant());

    if (object.progress.has_value())
        query.bindValue(QStringLiteral(":progress"), object.progress.value());
    else
        query.bindValue(QStringLiteral(":progress"), QVariant());

    query.bindValue(QStringLiteral(":updated_at"), QString::fromUtf8(object.updatedAt.c_str()));

    if (object.deletedAt.has_value())
        query.bindValue(QStringLiteral(":deleted_at"), QString::fromUtf8(object.deletedAt->c_str()));
    else
        query.bindValue(QStringLiteral(":deleted_at"), QVariant());

    if (object.formerObjectTypeUuid.has_value())
        query.bindValue(QStringLiteral(":former_object_type_uuid"), QString::fromUtf8(object.formerObjectTypeUuid->c_str()));
    else
        query.bindValue(QStringLiteral(":former_object_type_uuid"), QVariant());

    if (object.formerObjectTypeName.has_value())
        query.bindValue(QStringLiteral(":former_object_type_name"), QString::fromUtf8(object.formerObjectTypeName->c_str()));
    else
        query.bindValue(QStringLiteral(":former_object_type_name"), QVariant());

    if (object.deletedTimezone.has_value())
        query.bindValue(QStringLiteral(":deleted_timezone"), QString::fromUtf8(object.deletedTimezone->c_str()));
    else
        query.bindValue(QStringLiteral(":deleted_timezone"), QVariant());

    query.bindValue(QStringLiteral(":id"), QVariant::fromValue(object.id));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    return query.numRowsAffected() > 0;
}

bool ObjectRepository::removePermanently(std::int64_t id)
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral("DELETE FROM objects WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), QVariant::fromValue(id));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    return query.numRowsAffected() > 0;
}
