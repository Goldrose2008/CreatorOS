#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

enum class ObjectRelationRole
{
    Main,
    Addition,
    Support
};

std::string objectRelationRoleToString(ObjectRelationRole role);
std::optional<ObjectRelationRole> objectRelationRoleFromString(std::string_view value);

struct ObjectRelation
{
    std::int64_t id = 0;
    std::string uuid;

    std::int64_t parentObjectId = 0;
    std::int64_t childObjectId = 0;

    ObjectRelationRole role = ObjectRelationRole::Main;

    std::string createdAt;
    std::string updatedAt;
};