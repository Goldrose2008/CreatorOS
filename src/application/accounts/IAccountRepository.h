#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "../../domain/models/Account.h"

class IAccountRepository
{
public:
    virtual ~IAccountRepository() = default;

    virtual std::vector<Account> findAll() const = 0;
    virtual std::optional<Account> findById(std::int64_t id) const = 0;

    virtual std::int64_t create(const Account &account) = 0;
    virtual bool update(const Account &account) = 0;
    virtual bool remove(std::int64_t id) = 0;
};