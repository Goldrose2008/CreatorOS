#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "../../domain/models/Account.h"

class IAccountRepository;
class IPlatformRepository;

class AccountService
{
public:
    AccountService(IAccountRepository &repository, IPlatformRepository &platformRepository);

    std::vector<Account> getAllAccounts() const;
    std::optional<Account> getAccountByID(std::int64_t id) const;

    bool isAccountNameValid(const std::string &name) const;

    Account createAccount(const Account &sourceAccount);
    bool updateAccount(const Account &sourceAccount);
    bool deleteAccount(std::int64_t id) const;

private:
    void validatePlatform(std::int64_t platformId) const;
    void setAccountFields(Account &account, const Account &source) const;

    IAccountRepository &repository_;
    IPlatformRepository &platformRepository_;
};