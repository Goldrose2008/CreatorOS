#pragma once

#include <cstdint>
#include <optional>

struct PropertyTextSettings
{
    std::int64_t propertyId = 0;

    std::optional<std::int64_t> minLength;
    std::optional<std::int64_t> maxLength;
};