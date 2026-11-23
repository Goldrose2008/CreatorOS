#include "ObjectTypeRepository.h"

#include "DatabaseManager.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

#include <stdexcept>

namespace
{
    ObjectType readObjectType(const QSqlQuery &query)
    {
        ObjectType objectType;

        objectType.id = query.value("id").toLongLong();
        objectType.uuid = query.value("uuid").toString().toStdString();
        objectType.name = query.value("name").toString().toStdString();

        if (!query.value("description").isNull())
            objectType.description = query.value("description").toString().toStdString();

        objectType.createdAt = query.value("created_at").toString().toStdString();
        objectType.updatedAt = query.value("updated_at").toString().toStdString();

        return objectType;
    }

    QString selectColumns()
    {
        return QStringLiteral(
            "id, "
            "uuid, "
            "name, "
            "description, "
            "created_at, "
            "updated_at "
        );
    }
}

ObjectTypeRepository::ObjectTypeRepository(const DatabaseManager &databaseManager) : database_(databaseManager.database())
{
}

std::vector<ObjectType> ObjectTypeRepository::findAll() const
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral("SELECT ") + selectColumns() + QStringLiteral("FROM object_types ORDER BY name COLLATE NOCASE ASC, id ASC"));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    std::vector<ObjectType> objectTypes;

    while (query.next())
        objectTypes.push_back(readObjectType(query));

    return objectTypes;
}

std::optional<ObjectType> ObjectTypeRepository::findById(std::int64_t id) const
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral("SELECT ") + selectColumns() + QStringLiteral("FROM object_types WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), QVariant::fromValue(id));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    if (!query.next())
        return std::nullopt;

    return readObjectType(query);
}

bool ObjectTypeRepository::existsByName(const std::string &name, const std::optional<std::int64_t> &excludeId) const
{
    QSqlQuery query(database_);

    QString sql = QStringLiteral("SELECT 1 FROM object_types WHERE name = :name COLLATE NOCASE");

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

std::int64_t ObjectTypeRepository::create(const ObjectType &objectType)
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral(
        "INSERT INTO object_types "
        "(uuid, name, description, created_at, updated_at) "
        "VALUES (:uuid, :name, :description, :created_at, :updated_at)"
    ));
    query.bindValue(QStringLiteral(":uuid"), QString::fromUtf8(objectType.uuid.c_str()));
    query.bindValue(QStringLiteral(":name"), QString::fromUtf8(objectType.name.c_str()));

    if (objectType.description.has_value())
        query.bindValue(QStringLiteral(":description"), QString::fromUtf8(objectType.description->c_str()));
    else
        query.bindValue(QStringLiteral(":description"), QVariant());

    query.bindValue(QStringLiteral(":created_at"), QString::fromUtf8(objectType.createdAt.c_str()));
    query.bindValue(QStringLiteral(":updated_at"), QString::fromUtf8(objectType.updatedAt.c_str()));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    return query.lastInsertId().toLongLong();
}

bool ObjectTypeRepository::update(const ObjectType &objectType)
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral(
        "UPDATE object_types SET "
        "name = :name, "
        "description = :description, "
        "updated_at = :updated_at "
        "WHERE id = :id"
    ));

    query.bindValue(QStringLiteral(":name"), QString::fromUtf8(objectType.name.c_str()));

    if (objectType.description.has_value())
        query.bindValue(QStringLiteral(":description"), QString::fromUtf8(objectType.description->c_str()));
    else
        query.bindValue(QStringLiteral(":description"), QVariant());

    query.bindValue(QStringLiteral(":updated_at"), QString::fromUtf8(objectType.updatedAt.c_str()));
    query.bindValue(QStringLiteral(":id"), QVariant::fromValue(objectType.id));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    return query.numRowsAffected() > 0;
}

bool ObjectTypeRepository::remove(std::int64_t id)
{
    QSqlDatabase database = database_;

    if (!database.transaction())
        throw std::runtime_error(database.lastError().text().toStdString());

    QSqlQuery findQuery(database);

    findQuery.prepare(QStringLiteral("SELECT uuid, name FROM object_types WHERE id = :id"));
    findQuery.bindValue(QStringLiteral(":id"), QVariant::fromValue(id));

    if (!findQuery.exec())
    {
        database.rollback();
        throw std::runtime_error(findQuery.lastError().text().toStdString());
    }

    if (!findQuery.next())
    {
        if (!database.commit())
            throw std::runtime_error(database.lastError().text().toStdString());

        return false;
    }

    const QString formerUuid = findQuery.value("uuid").toString();
    const QString formerName = findQuery.value("name").toString();

    findQuery.finish();
    
    QSqlQuery updateObjectsQuery(database);

    updateObjectsQuery.prepare(QStringLiteral(
        "UPDATE objects SET "
        "object_type_id = NULL, "
        "former_object_type_uuid = :former_uuid, "
        "former_object_type_name = :former_name "
        "WHERE object_type_id = :object_type_id "
        "AND deleted_at IS NOT NULL"
    ));

    updateObjectsQuery.bindValue(QStringLiteral(":former_uuid"), formerUuid);
    updateObjectsQuery.bindValue(QStringLiteral(":former_name"), formerName);
    updateObjectsQuery.bindValue(QStringLiteral(":object_type_id"), QVariant::fromValue(id));

    if (!updateObjectsQuery.exec())
    {
        database.rollback();
        throw std::runtime_error(updateObjectsQuery.lastError().text().toStdString());
    }

    QSqlQuery deleteQuery(database);

    deleteQuery.prepare(QStringLiteral("DELETE FROM object_types WHERE id = :id"));
    deleteQuery.bindValue(QStringLiteral(":id"), QVariant::fromValue(id));

    if (!deleteQuery.exec())
    {
        database.rollback();
        throw std::runtime_error(deleteQuery.lastError().text().toStdString());
    }

    if (!database.commit())
        throw std::runtime_error(database.lastError().text().toStdString());

    return deleteQuery.numRowsAffected() > 0;
}