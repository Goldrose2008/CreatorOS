#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "../../domain/models/ObjectRelation.h"

class IObjectRelationRepository
{
public:
    virtual ~IObjectRelationRepository() = default;

    virtual std::optional<ObjectRelation> findById(std::int64_t id) const = 0;
    virtual std::vector<ObjectRelation> findByParentObjectId(std::int64_t parentObjectId) const = 0;
    virtual std::vector<ObjectRelation> findByChildObjectId(std::int64_t childObjectId) const = 0;

    virtual std::int64_t create(const ObjectRelation &relation) = 0;
    virtual bool update(const ObjectRelation &relation) = 0;
    virtual bool remove(std::int64_t id) = 0;
};