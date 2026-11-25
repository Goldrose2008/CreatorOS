#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "../../domain/models/ObjectRelation.h"

class IObjectRelationRepository;
class IObjectRepository;

class ObjectRelationService
{
public:
    ObjectRelationService(IObjectRelationRepository &repository, IObjectRepository &objectRepository);

    std::vector<ObjectRelation> getChildRelations(std::int64_t parentObjectId) const;
    std::vector<ObjectRelation> getParentRelations(std::int64_t childObjectId) const;
    std::optional<ObjectRelation> getRelationByID(std::int64_t id) const;

    ObjectRelation createRelation(std::int64_t parentObjectId, std::int64_t childObjectId, ObjectRelationRole role);
    bool updateRelationRole(std::int64_t id, ObjectRelationRole role);
    bool deleteRelation(std::int64_t id);

private:
    bool areObjectsActive(std::int64_t parentObjectId, std::int64_t childObjectId) const;
    bool wouldCreateCycle(std::int64_t parentObjectId, std::int64_t childObjectId) const;
    bool isRoleValid(ObjectRelationRole role) const;

    IObjectRelationRepository &repository_;
    IObjectRepository &objectRepository_;
};