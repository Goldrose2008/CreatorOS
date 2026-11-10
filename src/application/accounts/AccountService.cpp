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

std::vector<Account> AccountService::getAllAccounts() const
{
    return repository_.findAll();
}

std::optional<Account> AccountService::getAccountByID(std::int64_t id) const
{
    return CreatorServiceUtils::findEntityByID(repository_, id);
}

bool AccountService::isAccountNameValid(const std::string &name) const
{
    return !CreatorStringUtils::isBlank(name);
}

void AccountService::validatePlatform(std::int64_t platformId) const
{
    if (!CreatorServiceUtils::findEntityByID(platformRepository_, platformId).has_value())
        throw std::invalid_argument("Account platform was not found.");
}

void AccountService::setAccountFields(
    Account &account,
    std::int64_t platformId,
    const std::string &name,
    const std::string &username,
    const std::string &profileUrl,
    const std::string &externalId
) const
{
    if (!isAccountNameValid(name))
        throw std::invalid_argument("Account name cannot be empty.");

    account.platformId = platformId;
    account.name = CreatorStringUtils::trim(name);
    account.username = CreatorStringUtils::trim(username);
    account.profileUrl = CreatorStringUtils::trim(profileUrl);
    account.externalId = CreatorStringUtils::trim(externalId);
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
    setAccountFields(account, platformId, name, username, profileUrl, externalId);

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
    setAccountFields(account, platformId, name, username, profileUrl, externalId);  

    return repository_.update(account);
}

bool AccountService::deleteAccount(std::int64_t id) const
{
    if (id <= 0)
        return false;

    return repository_.remove(id);
}