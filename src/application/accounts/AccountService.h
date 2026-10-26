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
    AccountService(
        IAccountRepository &repository,
        IPlatformRepository &platformRepository
    );

    std::vector<Account> getAccounts() const;
    std::optional<Account> getAccount(std::int64_t id) const;

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

    IAccountRepository &repository_;
    IPlatformRepository &platformRepository_;
};