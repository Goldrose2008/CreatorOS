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

void AccountService::setAccountFields(Account &account, const Account &source) const
{
    if (!isAccountNameValid(source.name))
        throw std::invalid_argument("Account name cannot be empty.");

    if (!CreatorServiceUtils::findEntityByID(platformRepository_, source.platformId).has_value())
        throw std::invalid_argument("Account platform was not found.");

    account.platformId = source.platformId;
    account.name = CreatorStringUtils::trim(source.name);
    account.username = CreatorStringUtils::trim(source.username);
    account.profileUrl = CreatorStringUtils::trim(source.profileUrl);
    account.externalId = CreatorStringUtils::trim(source.externalId);
}

Account AccountService::createAccount(const Account &sourceAccount)
{
    Account account;
    setAccountFields(account, sourceAccount);

    return CreatorServiceUtils::createAndLoadEntity(
        repository_,
        "Created account could not be loaded.",
        account);
}

bool AccountService::updateAccount(const Account &sourceAccount)
{
    const auto existingAccount = CreatorServiceUtils::findEntityByID(repository_, sourceAccount.id);

    if (!existingAccount.has_value())
        return false;

    Account account = existingAccount.value();
    setAccountFields(account, sourceAccount);  

    return repository_.update(account);
}

bool AccountService::deleteAccount(std::int64_t id) const
{
    if (id <= 0)
        return false;

    return repository_.remove(id);
}