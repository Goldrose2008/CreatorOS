
#pragma once

#include <QSqlDatabase>

#include "../../application/objects/IObjectRepository.h"

class DatabaseManager;

class ObjectRepository final : public IObjectRepository
{
public:
    explicit ObjectRepository(const DatabaseManager &databaseManager);

    std::vector<Object> findAllActive() const override;
    std::vector<Object> findAllInTrash() const override;
    std::optional<Object> findById(std::int64_t id) const override;
    std::vector<Object> findActiveByObjectTypeId(std::int64_t objectTypeId) const override;

    bool existsActiveByName(const std::string &name, const std::optional<std::int64_t> &excludeId = std::nullopt) const override;

    std::int64_t create(const Object &object) override;
    bool update(const Object &object) override;
    bool removePermanently(std::int64_t id) override;

private:
    QSqlDatabase database_;
};
