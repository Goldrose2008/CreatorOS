#include "PropertyDefinition.h"

#include <stdexcept>

std::string propertyDefinitionScopeToDatabaseValue(PropertyDefinitionScope scope)
{
    switch (scope)
    {
    case PropertyDefinitionScope::Common:
        return "common";

    case PropertyDefinitionScope::ObjectType:
        return "object_type";
    }

    throw std::logic_error("Unknown PropertyDefinitionScope.");
}

std::optional<PropertyDefinitionScope> propertyDefinitionScopeFromDatabaseValue(std::string_view value)
{
    if (value == "common")
        return PropertyDefinitionScope::Common;

    if (value == "object_type")
        return PropertyDefinitionScope::ObjectType;

    return std::nullopt;
}

std::string propertyValueTypeToDatabaseValue(PropertyValueType type)
{
    switch (type)
    {
    case PropertyValueType::Text:
        return "text";

    case PropertyValueType::Number:
        return "number";

    case PropertyValueType::Boolean:
        return "boolean";

    case PropertyValueType::Date:
        return "date";

    case PropertyValueType::DateTime:
        return "datetime";

    case PropertyValueType::EntityReference:
        return "entity_reference";
    }

    throw std::logic_error("Unknown PropertyValueType.");
}

std::optional<PropertyValueType> propertyValueTypeFromDatabaseValue(std::string_view value)
{
    if (value == "text")
        return PropertyValueType::Text;

    if (value == "number")
        return PropertyValueType::Number;

    if (value == "boolean")
        return PropertyValueType::Boolean;

    if (value == "date")
        return PropertyValueType::Date;

    if (value == "datetime")
        return PropertyValueType::DateTime;

    if (value == "entity_reference")
        return PropertyValueType::EntityReference;

    return std::nullopt;
}