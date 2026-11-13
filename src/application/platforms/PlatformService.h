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
    
    Platform createPlatform(const Platform &sourcePlatform);
    bool updatePlatform(const Platform &sourcePlatform);
    bool deletePlatform(std::int64_t id) const;

private:
    void setPlatformFields(Platform &platform, const Platform &source) const;

    IPlatformRepository &repository_;
};