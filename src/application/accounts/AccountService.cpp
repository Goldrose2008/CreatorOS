#include "AccountService.h"

#include <stdexcept>

#include "IAccountRepository.h"
#include "../common/ServiceUtils.h"
#include "../common/StringUtils.h"
#include "../platforms/IPlatformRepository.h"

AccountService::AccountService(IAccountRepository &repository, IPlatformRepository &platformRepository)
    : repository_(repository), platformRepository_(platformRepository)
{
}

bool AccountService::isAccountNameValid(const std::string &name) const
{
    return !CreatorStringUtils::isBlank(name);
}

bool AccountService::isPlatformValid(std::int64_t platformId) const
{
    return CreatorServiceUtils::findEntityByID(platformRepository_, platformId).has_value();
}

std::vector<Account> AccountService::getAllAccounts() const
{
    return repository_.findAll();
}

std::optional<Account> AccountService::getAccountByID(std::int64_t id) const
{
    return CreatorServiceUtils::findEntityByID(repository_, id);
}

void AccountService::validatePlatform(std::int64_t platformId) const
{
    if (!isPlatformValid(platformId))
        throw std::invalid_argument("Account platform was not found.");
}

Account AccountService::createAccount(
    std::int64_t platformId,
    const std::string &name,
    const std::string &username,
    const std::string &profileUrl,
    const std::string &externalId
)
{
    validatePlatform(platformId);

    Account account;
    account.platformId = platformId;
    account.name = CreatorStringUtils::trim(name);
    account.username = CreatorStringUtils::trim(username);
    account.profileUrl = CreatorStringUtils::trim(profileUrl);
    account.externalId = CreatorStringUtils::trim(externalId);

    if (!isAccountNameValid(account.name))
        throw std::invalid_argument("Account name cannot be empty.");

    return CreatorServiceUtils::createAndLoadEntity(
        repository_,
        "Created account could not be loaded.",
        account);
}

bool AccountService::updateAccount(
    std::int64_t id,
    std::int64_t platformId,
    const std::string &name,
    const std::string &username,
    const std::string &profileUrl,
    const std::string &externalId
)
{
    const auto existing = CreatorServiceUtils::findEntityByID(repository_, id);

    if (!existing.has_value())
        return false;

    validatePlatform(platformId);

    Account account = existing.value();

    account.platformId = platformId;
    account.name = CreatorStringUtils::trim(name);
    account.username = CreatorStringUtils::trim(username);
    account.profileUrl = CreatorStringUtils::trim(profileUrl);
    account.externalId = CreatorStringUtils::trim(externalId);

    if (!isAccountNameValid(account.name))
        throw std::invalid_argument("Account name cannot be empty.");

    return repository_.update(account);
}

bool AccountService::deleteAccount(std::int64_t id) const
{
    if (id <= 0)
        return false;

    return repository_.remove(id);
}