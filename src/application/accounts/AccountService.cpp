#include "AccountService.h"

#include <stdexcept>

#include "IAccountRepository.h"
#include "../platforms/IPlatformRepository.h"
#include "../common/StringUtils.h"

AccountService::AccountService(IAccountRepository &repository, IPlatformRepository &platformRepository)
    : repository_(repository), platformRepository_(platformRepository)
{
}

std::vector<Account> AccountService::getAccounts() const
{
    return repository_.findAll();
}

std::optional<Account> AccountService::getAccount(std::int64_t id) const
{
    if (id <= 0)
        return std::nullopt;

    return repository_.findById(id);
}

void AccountService::validatePlatform(std::int64_t platformId) const
{
    if (platformId <= 0 || !platformRepository_.findById(platformId).has_value())
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

    if (account.name.empty())
        throw std::invalid_argument("Account name cannot be empty.");

    const std::int64_t id = repository_.create(account);
    const auto createdAccount = repository_.findById(id);

    if (!createdAccount.has_value())
        throw std::runtime_error("Created account could not be loaded.");

    return createdAccount.value();
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
    const auto existing = repository_.findById(id);

    if (!existing.has_value())
        return false;

    validatePlatform(platformId);

    Account account = existing.value();

    account.platformId = platformId;
    account.name = CreatorStringUtils::trim(name);
    account.username = CreatorStringUtils::trim(username);
    account.profileUrl = CreatorStringUtils::trim(profileUrl);
    account.externalId = CreatorStringUtils::trim(externalId);

    if (account.name.empty())
        throw std::invalid_argument("Account name cannot be empty.");

    return repository_.update(account);
}

bool AccountService::deleteAccount(std::int64_t id) const
{
    if (id <= 0)
        return false;

    return repository_.remove(id);
}