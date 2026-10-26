#pragma once

#include <cstdint>
#include <string>

struct Account
{
    std::int64_t id = 0;
    std::int64_t platformId = 0;
    std::string name;
    std::string username;
    std::string profileUrl;
    std::string externalId;
    std::string createdAt;
    std::string updatedAt;
};