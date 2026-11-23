#pragma once

#include <QSqlDatabase>

#include "../../application/object_types/IObjectTypeRepository.h"

class DatabaseManager;

class ObjectTypeRepository final : public IObjectTypeRepository
{
public:
    explicit ObjectTypeRepository(const DatabaseManager &databaseManager);

    std::vector<ObjectType> findAll() const override;
    std::optional<ObjectType> findById(std::int64_t id) const override;

    bool existsByName(const std::string &name, const std::optional<std::int64_t> &excludeId = std::nullopt) const override;

    std::int64_t create(const ObjectType &objectType) override;
    bool update(const ObjectType &objectType) override;
    bool remove(std::int64_t id) override;

private:
    QSqlDatabase database_;
};