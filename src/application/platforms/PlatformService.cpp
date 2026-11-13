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

bool PlatformService::hasAccounts(std::int64_t id) const
{
    if (id <= 0)
        return false;

    return repository_.hasAccounts(id);
}

bool PlatformService::isPlatformNameValid(const std::string &name) const
{
    return !CreatorStringUtils::isBlank(name);
}

void PlatformService::setPlatformFields(Platform &platform, const Platform &source) const
{
    if (!isPlatformNameValid(source.name))
        throw std::invalid_argument("Platform name cannot be empty.");

    platform.name = CreatorStringUtils::trim(source.name);
    platform.websiteUrl = CreatorStringUtils::trim(source.websiteUrl);
}

Platform PlatformService::createPlatform(const Platform &sourcePlatform)
{
    Platform platform;
    setPlatformFields(platform, sourcePlatform);

    return CreatorServiceUtils::createAndLoadEntity(
        repository_,
        "Created platform could not be loaded.",
        platform);
}

bool PlatformService::updatePlatform(const Platform &sourcePlatform)
{
    const auto existing = CreatorServiceUtils::findEntityByID(repository_, sourcePlatform.id);

    if (!existing.has_value())
        return false;

    Platform platform = existing.value();
    setPlatformFields(platform, sourcePlatform);

    return repository_.update(platform);
}

bool PlatformService::deletePlatform(std::int64_t id) const
{
    if (id <= 0 || repository_.hasAccounts(id))
        return false;

    return repository_.remove(id);
}