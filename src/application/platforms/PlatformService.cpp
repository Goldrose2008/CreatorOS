#include "PlatformService.h"

#include <stdexcept>

#include "IPlatformRepository.h"
#include "../common/ServiceUtils.h"
#include "../common/StringUtils.h"

PlatformService::PlatformService(IPlatformRepository &repository) : repository_(repository)
{
}

std::vector<Platform> PlatformService::getAllPlatforms() const
{
    return repository_.findAll();
}

std::optional<Platform> PlatformService::getPlatformByID(std::int64_t id) const
{
    return CreatorServiceUtils::findEntityByID(repository_, id);
}

Platform PlatformService::createPlatform(const std::string &name, const std::string &websiteUrl)
{
    Platform platform;

    platform.name = CreatorStringUtils::trim(name);
    platform.websiteUrl = CreatorStringUtils::trim(websiteUrl);

    if (platform.name.empty())
        throw std::invalid_argument("Platform name cannot be empty.");

    return CreatorServiceUtils::createAndLoadEntity(
        repository_,
        "Created platform could not be loaded.",
        platform);
}

bool PlatformService::updatePlatform(std::int64_t id, const std::string &name, const std::string &websiteUrl)
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