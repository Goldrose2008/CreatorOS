#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "../../domain/models/Platform.h"

class IPlatformRepository;

class PlatformService
{
public:
    explicit PlatformService(IPlatformRepository &repository);

    std::vector<Platform> getAllPlatforms() const;
    std::optional<Platform> getPlatformByID(std::int64_t id) const;

    bool hasAccounts(std::int64_t id) const;
    bool isPlatformNameValid(const std::string &name) const;
    
    Platform createPlatform(const std::string &name, const std::string &websiteUrl);
    bool updatePlatform(std::int64_t id, const std::string &name, const std::string &websiteUrl);
    bool deletePlatform(std::int64_t id) const;

private:
    IPlatformRepository &repository_;
};