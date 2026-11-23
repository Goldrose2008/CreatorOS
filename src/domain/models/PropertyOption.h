#pragma once

#include <cstdint>
#include <string>

struct PropertyOption
{
    std::int64_t id = 0;
    std::int64_t propertyId = 0;

    std::string value;
    std::string normalizedValue;
};