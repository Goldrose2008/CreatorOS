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

bool PropertyValueRepository::save(const PropertyValue &value, const std::string &updatedAt)
{
    QSqlDatabase database = database_;

    if (!database.transaction())
        throw std::runtime_error(database.lastError().text().toStdString());

    try
    {
        QSqlQuery query(database);

        query.prepare(QStringLiteral(
            "INSERT INTO property_values (object_id, property_id, value) "
            "VALUES (:object_id, :property_id, :value) "
            "ON CONFLICT(object_id, property_id) DO UPDATE SET value = excluded.value"
        ));
        query.bindValue(QStringLiteral(":object_id"), QVariant::fromValue(value.objectId));
        query.bindValue(QStringLiteral(":property_id"), QVariant::fromValue(value.propertyId));
        query.bindValue(QStringLiteral(":value"), QString::fromUtf8(value.value.c_str()));

        if (!query.exec())
            throw std::runtime_error(query.lastError().text().toStdString());

        query.finish();

        QSqlQuery updateObjectQuery(database);
        updateObjectQuery.prepare(QStringLiteral("UPDATE objects SET updated_at = :updated_at WHERE id = :object_id"));
        updateObjectQuery.bindValue(QStringLiteral(":updated_at"), QString::fromUtf8(updatedAt.c_str()));
        updateObjectQuery.bindValue(QStringLiteral(":object_id"), QVariant::fromValue(value.objectId));

        if (!updateObjectQuery.exec())
            throw std::runtime_error(updateObjectQuery.lastError().text().toStdString());

        if (updateObjectQuery.numRowsAffected() <= 0)
            throw std::runtime_error("Object could not be updated.");

        if (!database.commit())
            throw std::runtime_error(database.lastError().text().toStdString());

        return true;
    }
    catch (...)
    {
        database.rollback();
        throw;
    }
}

bool PropertyValueRepository::remove(std::int64_t objectId, std::int64_t propertyId, const std::string &updatedAt)
{
    QSqlDatabase database = database_;

    if (!database.transaction())
        throw std::runtime_error(database.lastError().text().toStdString());

    try
    {
        QSqlQuery query(database);

        query.prepare(QStringLiteral("DELETE FROM property_values "
            "WHERE object_id = :object_id AND property_id = :property_id"));
        query.bindValue(QStringLiteral(":object_id"), QVariant::fromValue(objectId));
        query.bindValue(QStringLiteral(":property_id"), QVariant::fromValue(propertyId));

        if (!query.exec())
            throw std::runtime_error(query.lastError().text().toStdString());

        const bool wasRemoved = query.numRowsAffected() > 0;

        query.finish();

        // Если значения не было, Object не изменился.
        if (!wasRemoved)
        {
            if (!database.commit())
                throw std::runtime_error(database.lastError().text().toStdString());

            return false;
        }

        QSqlQuery updateObjectQuery(database);

        updateObjectQuery.prepare(QStringLiteral("UPDATE objects SET updated_at = :updated_at WHERE id = :object_id"));
        updateObjectQuery.bindValue(QStringLiteral(":updated_at"), QString::fromUtf8(updatedAt.c_str()));
        updateObjectQuery.bindValue(QStringLiteral(":object_id"), QVariant::fromValue(objectId));

        if (!updateObjectQuery.exec())
            throw std::runtime_error(updateObjectQuery.lastError().text().toStdString());

        if (updateObjectQuery.numRowsAffected() <= 0)
            throw std::runtime_error("Object could not be updated.");

        if (!database.commit())
            throw std::runtime_error(database.lastError().text().toStdString());

        return true;
    }
    catch (...)
    {
        database.rollback();
        throw;
    }
}