#pragma once

#include <QSqlDatabase>

#include "../../application/accounts/IAccountRepository.h"

class DatabaseManager;

class AccountRepository final : public IAccountRepository
{
public:
    explicit AccountRepository(const DatabaseManager &databaseManager);

    std::vector<Account> findAll() const override;
    std::optional<Account> findById(std::int64_t id) const override;

    std::int64_t create(const Account &account) override;
    bool update(const Account &account) override;
    bool remove(std::int64_t id) override;

private:
    QSqlDatabase database_;
};