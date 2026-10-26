#pragma once

#include <cstdint>
#include <string>

struct Platform
{
    std::int64_t id = 0;
    std::string name;
    std::string websiteUrl;
    std::string createdAt;
    std::string updatedAt;
};