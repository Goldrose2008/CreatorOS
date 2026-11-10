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

    Account createAccount(
        std::int64_t platformId,
        const std::string &name,
        const std::string &username,
        const std::string &profileUrl,
        const std::string &externalId
    );

    bool updateAccount(
        std::int64_t id,
        std::int64_t platformId,
        const std::string &name,
        const std::string &username,
        const std::string &profileUrl,
        const std::string &externalId
    );

    bool deleteAccount(std::int64_t id) const;

private:
    void validatePlatform(std::int64_t platformId) const;
    void setAccountFields(
        Account &account,
        std::int64_t platformId,
        const std::string &name,
        const std::string &username,
        const std::string &profileUrl,
        const std::string &externalId
    ) const;

    IAccountRepository &repository_;
    IPlatformRepository &platformRepository_;
};