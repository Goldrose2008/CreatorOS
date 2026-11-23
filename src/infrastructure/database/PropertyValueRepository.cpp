#include "PropertyValueRepository.h"

#include "DatabaseManager.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <stdexcept>

namespace
{
    PropertyValue readPropertyValue(const QSqlQuery &query)
    {
        PropertyValue value;

        value.objectId = query.value("object_id").toLongLong();
        value.propertyId = query.value("property_id").toLongLong();
        value.value = query.value("value").toString().toStdString();

        return value;
    }
}

PropertyValueRepository::PropertyValueRepository(const DatabaseManager &databaseManager) : database_(databaseManager.database())
{
}

std::vector<PropertyValue> PropertyValueRepository::findByObjectId(std::int64_t objectId) const
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral(
        "SELECT object_id, property_id, value "
        "FROM property_values "
        "WHERE object_id = :object_id "
        "ORDER BY property_id ASC"
    ));
    query.bindValue(QStringLiteral(":object_id"), QVariant::fromValue(objectId));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    std::vector<PropertyValue> values;

    while (query.next())
        values.push_back(readPropertyValue(query));

    return values;
}

std::optional<PropertyValue>
PropertyValueRepository::findByObjectAndProperty(std::int64_t objectId, std::int64_t propertyId) const
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral(
        "SELECT object_id, property_id, value "
        "FROM property_values "
        "WHERE object_id = :object_id "
        "AND property_id = :property_id"
    ));
    query.bindValue(QStringLiteral(":object_id"), QVariant::fromValue(objectId));
    query.bindValue(QStringLiteral(":property_id"), QVariant::fromValue(propertyId));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    if (!query.next())
        return std::nullopt;

    return readPropertyValue(query);
}

bool PropertyValueRepository::save(const PropertyValue &value)
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral(
        "INSERT INTO property_values (object_id, property_id, value) "
        "VALUES (:object_id, :property_id, :value) "
        "ON CONFLICT(object_id, property_id) DO UPDATE SET "
        "value = excluded.value"
    ));
    query.bindValue(QStringLiteral(":object_id"), QVariant::fromValue(value.objectId));
    query.bindValue(QStringLiteral(":property_id"), QVariant::fromValue(value.propertyId));
    query.bindValue(QStringLiteral(":value"), QString::fromUtf8(value.value.c_str()));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    return query.numRowsAffected() > 0;
}

bool PropertyValueRepository::remove(std::int64_t objectId, std::int64_t propertyId)
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral(
        "DELETE FROM property_values "
        "WHERE object_id = :object_id "
        "AND property_id = :property_id"
    ));
    query.bindValue(QStringLiteral(":object_id"), QVariant::fromValue(objectId));
    query.bindValue(QStringLiteral(":property_id"), QVariant::fromValue(propertyId));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    return query.numRowsAffected() > 0;
}