#pragma once

#include <QSqlDatabase>

#include "../../application/platforms/IPlatformRepository.h"

class DatabaseManager;

class PlatformRepository final : public IPlatformRepository
{
public:
    explicit PlatformRepository(const DatabaseManager &databaseManager);

    std::vector<Platform> findAll() const override;
    std::optional<Platform> findById(std::int64_t id) const override;
    bool hasAccounts(std::int64_t id) const override;

    std::int64_t create(const Platform &platform) override;
    bool update(const Platform &platform) override;
    bool remove(std::int64_t id) override;

private:
    QSqlDatabase database_;
};