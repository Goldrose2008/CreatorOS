#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "../../domain/models/Object.h"
#include "../../domain/models/PropertyDefinition.h"
#include "../../domain/models/PropertyEntityReferenceSettings.h"
#include "../../domain/models/PropertyTextSettings.h"
#include "../../domain/models/PropertyValue.h"

class IPropertyValueRepository;
class IPropertyDefinitionRepository;
class IObjectRepository;

class PropertyValueService
{
public:
    PropertyValueService(IPropertyValueRepository &valueRepository, IPropertyDefinitionRepository &definitionRepository, IObjectRepository &objectRepository);

    std::vector<PropertyValue> getPropertyValuesForObject(std::int64_t objectId) const;
    std::optional<PropertyValue> getPropertyValue(std::int64_t objectId, std::int64_t propertyId) const;

    bool savePropertyValue(std::int64_t objectId, std::int64_t propertyId, const std::string &sourceValue);
    bool removePropertyValue(std::int64_t objectId, std::int64_t propertyId);

private:
    void validatePropertyApplicability(const PropertyDefinition &definition, const Object &object) const;

    std::optional<std::string> validateAndNormalizeValue(
        const PropertyDefinition &definition,
        const std::optional<PropertyTextSettings> &textSettings,
        const std::optional<PropertyEntityReferenceSettings> &entityReferenceSettings,
        const std::string &sourceValue
    ) const;

    IPropertyValueRepository &valueRepository_;
    IPropertyDefinitionRepository &definitionRepository_;
    IObjectRepository &objectRepository_;
};