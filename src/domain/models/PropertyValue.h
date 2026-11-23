#pragma once

#include <cstdint>
#include <string>

struct PropertyValue
{
    std::int64_t objectId = 0;
    std::int64_t propertyId = 0;

    std::string value;
};