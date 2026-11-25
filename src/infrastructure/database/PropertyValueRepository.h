#pragma once

#include <QSqlDatabase>

#include "../../application/properties/IPropertyValueRepository.h"

class DatabaseManager;

class PropertyValueRepository final : public IPropertyValueRepository
{
public:
    explicit PropertyValueRepository(const DatabaseManager &databaseManager);

    std::vector<PropertyValue> findByObjectId(std::int64_t objectId) const override;
    std::optional<PropertyValue> findByObjectAndProperty(std::int64_t objectId, std::int64_t propertyId) const override;

    bool save(const PropertyValue &value, const std::string &updatedAt) override;
    bool remove(std::int64_t objectId, std::int64_t propertyId, const std::string &updatedAt) override;

private:
    QSqlDatabase database_;
};