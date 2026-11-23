
#include "ObjectRelationRepository.h"

#include "DatabaseManager.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <stdexcept>

namespace
{
    ObjectRelation readObjectRelation(const QSqlQuery &query)
    {
        ObjectRelation relation;

        relation.id = query.value("id").toLongLong();
        relation.uuid = query.value("uuid").toString().toStdString();

        relation.parentObjectId = query.value("parent_object_id").toLongLong();
        relation.childObjectId = query.value("child_object_id").toLongLong();

        const std::string roleValue = query.value("role").toString().toStdString();
        const auto role = objectRelationRoleFromDatabaseValue(roleValue);

        if (!role.has_value())
            throw std::runtime_error("Unknown ObjectRelation role in database.");

        relation.role = role.value();

        relation.createdAt = query.value("created_at").toString().toStdString();
        relation.updatedAt = query.value("updated_at").toString().toStdString();

        return relation;
    }

    QString selectColumns()
    {
        return QStringLiteral(
            "id, "
            "uuid, "
            "parent_object_id, "
            "child_object_id, "
            "role, "
            "created_at, "
            "updated_at "
        );
    }
}

ObjectRelationRepository::ObjectRelationRepository(const DatabaseManager &databaseManager) : database_(databaseManager.database())
{
}

std::optional<ObjectRelation> ObjectRelationRepository::findById(std::int64_t id) const
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral("SELECT ") + selectColumns() + QStringLiteral("FROM object_relations WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), QVariant::fromValue(id));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    if (!query.next())
        return std::nullopt;

    return readObjectRelation(query);
}

std::vector<ObjectRelation> ObjectRelationRepository::findByParentObjectId(std::int64_t parentObjectId) const
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral("SELECT ") + selectColumns() + QStringLiteral(
            "FROM object_relations "
            "WHERE parent_object_id = :parent_object_id "
            "ORDER BY id ASC"
        )
    );

    query.bindValue(QStringLiteral(":parent_object_id"), QVariant::fromValue(parentObjectId));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    std::vector<ObjectRelation> relations;

    while (query.next())
        relations.push_back(readObjectRelation(query));

    return relations;
}

std::vector<ObjectRelation> ObjectRelationRepository::findByChildObjectId(std::int64_t childObjectId) const
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral("SELECT ") + selectColumns() + QStringLiteral(
            "FROM object_relations "
            "WHERE child_object_id = :child_object_id "
            "ORDER BY id ASC"
        )
    );

    query.bindValue(QStringLiteral(":child_object_id"), QVariant::fromValue(childObjectId));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    std::vector<ObjectRelation> relations;

    while (query.next())
        relations.push_back(readObjectRelation(query));

    return relations;
}

std::int64_t ObjectRelationRepository::create(const ObjectRelation &relation)
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral(
        "INSERT INTO object_relations ("
        "uuid, "
        "parent_object_id, "
        "child_object_id, "
        "role, "
        "created_at, "
        "updated_at"
        ") VALUES ("
        ":uuid, "
        ":parent_object_id, "
        ":child_object_id, "
        ":role, "
        ":created_at, "
        ":updated_at"
        ")"
    ));

    query.bindValue(QStringLiteral(":uuid"), QString::fromUtf8(relation.uuid.c_str()));
    query.bindValue(QStringLiteral(":parent_object_id"), QVariant::fromValue(relation.parentObjectId));
    query.bindValue(QStringLiteral(":child_object_id"), QVariant::fromValue(relation.childObjectId));

    const std::string roleValue = objectRelationRoleToDatabaseValue(relation.role);

    query.bindValue(QStringLiteral(":role"), QString::fromUtf8(roleValue.c_str()));
    query.bindValue(QStringLiteral(":created_at"), QString::fromUtf8(relation.createdAt.c_str()));
    query.bindValue(QStringLiteral(":updated_at"), QString::fromUtf8(relation.updatedAt.c_str()));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    return query.lastInsertId().toLongLong();
}

bool ObjectRelationRepository::update(const ObjectRelation &relation)
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral(
        "UPDATE object_relations SET "
        "role = :role, "
        "updated_at = :updated_at "
        "WHERE id = :id"
    ));

    const std::string roleValue = objectRelationRoleToDatabaseValue(relation.role);

    query.bindValue(QStringLiteral(":role"), QString::fromUtf8(roleValue.c_str()));
    query.bindValue(QStringLiteral(":updated_at"), QString::fromUtf8(relation.updatedAt.c_str()));
    query.bindValue(QStringLiteral(":id"), QVariant::fromValue(relation.id));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    return query.numRowsAffected() > 0;
}

bool ObjectRelationRepository::remove(std::int64_t id)
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral("DELETE FROM object_relations WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), QVariant::fromValue(id));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    return query.numRowsAffected() > 0;
}
