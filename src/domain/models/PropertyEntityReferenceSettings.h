#pragma once

#include <cstdint>
#include <string>

struct PropertyEntityReferenceSettings
{
    std::int64_t propertyId = 0;

    std::string targetEntityType;
};