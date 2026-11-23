
#pragma once

#include <QSqlDatabase>

#include "../../application/object_relations/IObjectRelationRepository.h"

class DatabaseManager;

class ObjectRelationRepository final : public IObjectRelationRepository
{
public:
    explicit ObjectRelationRepository(const DatabaseManager &databaseManager);

    std::optional<ObjectRelation> findById(std::int64_t id) const override;
    std::vector<ObjectRelation> findByParentObjectId(std::int64_t parentObjectId) const override;
    std::vector<ObjectRelation> findByChildObjectId(std::int64_t childObjectId) const override;

    std::int64_t create(const ObjectRelation &relation) override;
    bool update(const ObjectRelation &relation) override;
    bool remove(std::int64_t id) override;

private:
    QSqlDatabase database_;
};
