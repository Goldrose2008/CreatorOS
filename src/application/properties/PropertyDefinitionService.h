#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "../../domain/models/PropertyDefinition.h"
#include "../../domain/models/PropertyEntityReferenceSettings.h"
#include "../../domain/models/PropertyOption.h"
#include "../../domain/models/PropertyTextSettings.h"

class IPropertyDefinitionRepository;
class IObjectTypeRepository;

class PropertyDefinitionService
{
public:
    PropertyDefinitionService(
        IPropertyDefinitionRepository &repository,
        IObjectTypeRepository &objectTypeRepository
    );

    std::vector<PropertyDefinition> getCommonProperties() const;
    std::vector<PropertyDefinition> getPropertiesByObjectTypeID(std::int64_t objectTypeId) const;
    std::optional<PropertyDefinition> getPropertyDefinitionByID(std::int64_t id) const;
    std::optional<PropertyTextSettings> getTextSettings(std::int64_t propertyId) const;
    std::optional<PropertyEntityReferenceSettings> getEntityReferenceSettings(std::int64_t propertyId) const;
    std::vector<PropertyOption> getOptions(std::int64_t propertyId) const;

    bool isPropertyNameValid(
        const std::string &name,
        PropertyDefinitionScope scope,
        const std::optional<std::int64_t> &objectTypeId,
        const std::optional<std::int64_t> &excludeId = std::nullopt
    ) const;

    PropertyDefinition createPropertyDefinition(
        const PropertyDefinition &sourceDefinition,
        const std::optional<PropertyTextSettings> &textSettings,
        const std::optional<PropertyEntityReferenceSettings> &entityReferenceSettings,
        const std::vector<PropertyOption> &options
    );
    bool updatePropertyDefinition(
        const PropertyDefinition &sourceDefinition,
        const std::optional<PropertyTextSettings> &textSettings,
        const std::optional<PropertyEntityReferenceSettings> &entityReferenceSettings,
        const std::vector<PropertyOption> &options
    );
    bool deletePropertyDefinition(std::int64_t id) const;

private:
    void setPropertyDefinitionFields(PropertyDefinition &definition, const PropertyDefinition &source) const;
    void validateAndNormalizeDefinitionData(
        PropertyDefinition &definition,
        std::optional<PropertyTextSettings> &textSettings,
        std::optional<PropertyEntityReferenceSettings> &entityReferenceSettings,
        std::vector<PropertyOption> &options
    ) const;

    IPropertyDefinitionRepository &repository_;
    IObjectTypeRepository &objectTypeRepository_;
};