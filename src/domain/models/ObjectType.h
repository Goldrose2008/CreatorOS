#pragma once

#include <cstdint>
#include <optional>
#include <string>

struct ObjectType
{
    std::int64_t id = 0;
    std::string uuid;

    std::string name;
    std::optional<std::string> description;

    std::string createdAt;
    std::string updatedAt;
};