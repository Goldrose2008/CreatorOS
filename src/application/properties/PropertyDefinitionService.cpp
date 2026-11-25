#include "PropertyDefinitionService.h"

#include <QDateTime>
#include <QUuid>

#include <stdexcept>

#include "IPropertyDefinitionRepository.h"
#include "../common/ServiceUtils.h"
#include "../common/StringUtils.h"
#include "../object_types/IObjectTypeRepository.h"

PropertyDefinitionService::PropertyDefinitionService(IPropertyDefinitionRepository &repository, IObjectTypeRepository &objectTypeRepository)
    : repository_(repository), objectTypeRepository_(objectTypeRepository)
{
}

std::vector<PropertyDefinition> PropertyDefinitionService::getCommonProperties() const
{
    return repository_.findCommon();
}

std::vector<PropertyDefinition> PropertyDefinitionService::getPropertiesByObjectTypeID(std::int64_t objectTypeId) const
{
    if (!CreatorServiceUtils::findEntityByID(objectTypeRepository_, objectTypeId).has_value())
        return {};

    return repository_.findByObjectTypeId(objectTypeId);
}

std::optional<PropertyDefinition> PropertyDefinitionService::getPropertyDefinitionByID(std::int64_t id) const
{
    return CreatorServiceUtils::findEntityByID(repository_, id);
}

std::optional<PropertyTextSettings> PropertyDefinitionService::getTextSettings(std::int64_t propertyId) const
{
    if (propertyId <= 0)
        return std::nullopt;

    return repository_.findTextSettings(propertyId);
}

std::optional<PropertyEntityReferenceSettings> PropertyDefinitionService::getEntityReferenceSettings(std::int64_t propertyId) const
{
    if (propertyId <= 0)
        return std::nullopt;

    return repository_.findEntityReferenceSettings(propertyId);
}

std::vector<PropertyOption> PropertyDefinitionService::getOptions(std::int64_t propertyId) const
{
    if (propertyId <= 0)
        return {};

    return repository_.findOptions(propertyId);
}

bool PropertyDefinitionService::isPropertyNameValid(
    const std::string &name,
    PropertyDefinitionScope scope,
    const std::optional<std::int64_t> &objectTypeId,
    const std::optional<std::int64_t> &excludeId
) const
{
    const std::string normalizedName = CreatorStringUtils::trim(name);

    if (normalizedName.empty())
        return false;

    if (excludeId.has_value() && excludeId.value() <= 0)
        return false;

    switch (scope)
    {
    case PropertyDefinitionScope::Common:
        if (objectTypeId.has_value())
            return false;
        break;

    case PropertyDefinitionScope::ObjectType:
        if (!objectTypeId.has_value() || objectTypeId.value() <= 0)
            return false;

        if (!CreatorServiceUtils::findEntityByID(objectTypeRepository_, objectTypeId.value()).has_value())
            return false;
        break;

    default:
        return false;
    }

    return !repository_.existsByName(normalizedName, scope, objectTypeId, excludeId);
}

void PropertyDefinitionService::setPropertyDefinitionFields(PropertyDefinition &definition, const PropertyDefinition &source) const
{
    if (source.scope == PropertyDefinitionScope::Common)
    {
        if (source.objectTypeId.has_value())
            throw std::invalid_argument("Common property cannot belong to an object type.");
    }
    else if (source.scope == PropertyDefinitionScope::ObjectType)
    {
        if (!source.objectTypeId.has_value() || source.objectTypeId.value() <= 0)
            throw std::invalid_argument("Object type is required for this property.");

        if (!CreatorServiceUtils::findEntityByID(objectTypeRepository_, source.objectTypeId.value()).has_value())
            throw std::invalid_argument("Object type was not found.");
    }
    else
    {
        throw std::invalid_argument("Unknown property scope.");
    }

    const std::string normalizedName = CreatorStringUtils::trim(source.name);

    if (normalizedName.empty())
        throw std::invalid_argument("Property name cannot be empty.");

    if (!isPropertyNameValid(
        normalizedName,
        source.scope,
        source.objectTypeId,
        definition.id > 0 ? std::optional<std::int64_t>(definition.id) : std::nullopt
    ))
    {
        throw std::invalid_argument("Property name already exists.");
    }

    definition.scope = source.scope;
    definition.objectTypeId = source.objectTypeId;
    definition.name = normalizedName;
    definition.description = source.description;
    definition.valueType = source.valueType;
    definition.isMultiple = source.isMultiple;
    definition.isRequired = source.isRequired;
    definition.defaultValue = source.defaultValue;
    definition.isEnabled = source.isEnabled;
    definition.showOnCard = source.showOnCard;
    definition.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs).toStdString();
}








