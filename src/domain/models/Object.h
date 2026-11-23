#pragma once

#include <cstdint>
#include <optional>
#include <string>

struct Object
{
    std::int64_t id = 0;
    std::string uuid;

    std::optional<std::int64_t> objectTypeId;

    std::string name;
    std::optional<std::string> description;

    std::optional<std::string> deadlineAt;
    std::optional<double> progress;

    std::string createdAt;
    std::string updatedAt;

    std::optional<std::string> deletedAt;

    std::optional<std::string> formerObjectTypeUuid;
    std::optional<std::string> formerObjectTypeName;
    std::optional<std::string> deletedTimezone;
};