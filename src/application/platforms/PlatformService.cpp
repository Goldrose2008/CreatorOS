#include "PlatformService.h"

#include <stdexcept>

#include "IPlatformRepository.h"
#include "../common/StringUtils.h"

PlatformService::PlatformService(IPlatformRepository &repository) : repository_(repository)
{
}

std::vector<Platform> PlatformService::getPlatforms() const
{
    return repository_.findAll();
}

std::optional<Platform> PlatformService::getPlatform(std::int64_t id) const
{
    if (id <= 0)
        return std::nullopt;

    return repository_.findById(id);
}

Platform PlatformService::createPlatform(const std::string &name, const std::string &websiteUrl)
{
    Platform platform;

    platform.name = CreatorStringUtils::trim(name);
    platform.websiteUrl = CreatorStringUtils::trim(websiteUrl);

    if (platform.name.empty())
        throw std::invalid_argument("Platform name cannot be empty.");

    const std::int64_t id = repository_.create(platform);
    const auto createdPlatform = repository_.findById(id);

    if (!createdPlatform.has_value())
        throw std::runtime_error("Created platform could not be loaded.");

    return createdPlatform.value();
}

bool PlatformService::updatePlatform(
    std::int64_t id,
    const std::string &name,
    const std::string &websiteUrl
)
{
    const auto existing = repository_.findById(id);

    if (!existing.has_value())
        return false;

    Platform platform = existing.value();
    platform.name = CreatorStringUtils::trim(name);
    platform.websiteUrl = CreatorStringUtils::trim(websiteUrl);

    if (platform.name.empty())
        throw std::invalid_argument("Platform name cannot be empty.");

    return repository_.update(platform);
}

bool PlatformService::hasAccounts(std::int64_t id) const
{
    if (id <= 0)
        return false;

    return repository_.hasAccounts(id);
}

bool PlatformService::deletePlatform(std::int64_t id) const
{
    if (id <= 0 || repository_.hasAccounts(id))
        return false;

    return repository_.remove(id);
}