#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

enum class PropertyDefinitionScope
{
    Common,
    ObjectType
};

enum class PropertyValueType
{
    Text,
    Number,
    Boolean,
    Date,
    DateTime,
    EntityReference
};

std::string propertyDefinitionScopeToDatabaseValue(PropertyDefinitionScope scope);
std::optional<PropertyDefinitionScope> propertyDefinitionScopeFromDatabaseValue(std::string_view value);

std::string propertyValueTypeToDatabaseValue(PropertyValueType type);
std::optional<PropertyValueType> propertyValueTypeFromDatabaseValue(std::string_view value);

struct PropertyDefinition
{
    std::int64_t id = 0;
    std::string uuid;

    PropertyDefinitionScope scope = PropertyDefinitionScope::Common;
    std::optional<std::int64_t> objectTypeId;

    std::string name;
    std::optional<std::string> description;

    PropertyValueType valueType = PropertyValueType::Text;

    bool isMultiple = false;
    bool isRequired = false;

    std::optional<std::string> defaultValue;

    bool isEnabled = true;
    bool showOnCard = false;

    std::string createdAt;
    std::string updatedAt;
};