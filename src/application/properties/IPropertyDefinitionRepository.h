#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "../../domain/models/PropertyDefinition.h"
#include "../../domain/models/PropertyEntityReferenceSettings.h"
#include "../../domain/models/PropertyOption.h"
#include "../../domain/models/PropertyTextSettings.h"

class IPropertyDefinitionRepository
{
public:
    virtual ~IPropertyDefinitionRepository() = default;

    virtual std::vector<PropertyDefinition> findCommon() const = 0;
    virtual std::vector<PropertyDefinition> findByObjectTypeId(std::int64_t objectTypeId) const = 0;
    virtual std::optional<PropertyDefinition> findById(std::int64_t id) const = 0;

    virtual bool existsByName(
        const std::string &name,
        PropertyDefinitionScope scope,
        const std::optional<std::int64_t> &objectTypeId,
        const std::optional<std::int64_t> &excludeId = std::nullopt
    ) const = 0;

    virtual std::int64_t create(
        const PropertyDefinition &definition,
        const std::optional<PropertyTextSettings> &textSettings,
        const std::optional<PropertyEntityReferenceSettings> &entityReferenceSettings,
        const std::vector<PropertyOption> &options
    ) = 0;
    virtual bool update(
        const PropertyDefinition &definition,
        const std::optional<PropertyTextSettings> &textSettings,
        const std::optional<PropertyEntityReferenceSettings> &entityReferenceSettings,
        const std::vector<PropertyOption> &options
    ) = 0;
    virtual bool remove(std::int64_t id) = 0;

    virtual std::optional<PropertyTextSettings> findTextSettings(std::int64_t propertyId) const = 0;
    virtual std::optional<PropertyEntityReferenceSettings> findEntityReferenceSettings(std::int64_t propertyId) const = 0;
    virtual std::vector<PropertyOption> findOptions(std::int64_t propertyId) const = 0;
};