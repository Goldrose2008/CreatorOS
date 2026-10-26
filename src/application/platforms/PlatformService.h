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

    std::vector<Platform> getPlatforms() const;
    std::optional<Platform> getPlatform(std::int64_t id) const;

    Platform createPlatform(
        const std::string &name,
        const std::string &websiteUrl
    );

    bool updatePlatform(
        std::int64_t id,
        const std::string &name,
        const std::string &websiteUrl
    );

    bool hasAccounts(std::int64_t id) const;
    bool deletePlatform(std::int64_t id) const;

private:
    IPlatformRepository &repository_;
};