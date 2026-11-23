#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "../../domain/models/PropertyValue.h"

class IPropertyValueRepository
{
public:
    virtual ~IPropertyValueRepository() = default;

    virtual std::vector<PropertyValue> findByObjectId(std::int64_t objectId) const = 0;
    virtual std::optional<PropertyValue> findByObjectAndProperty(std::int64_t objectId, std::int64_t propertyId) const = 0;

    virtual bool save(const PropertyValue &value) = 0;
    virtual bool remove(std::int64_t objectId, std::int64_t propertyId) = 0;
};