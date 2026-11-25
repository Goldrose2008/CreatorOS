#include "ObjectRelationService.h"

#include <QDateTime>
#include <QUuid>

#include <algorithm>
#include <stdexcept>
#include <unordered_set>

#include "IObjectRelationRepository.h"
#include "../objects/IObjectRepository.h"
#include "../common/ServiceUtils.h"

ObjectRelationService::ObjectRelationService(IObjectRelationRepository &repository, IObjectRepository &objectRepository)
    : repository_(repository), objectRepository_(objectRepository)
{
}

bool ObjectRelationService::areObjectsActive(std::int64_t parentObjectId, std::int64_t childObjectId) const
{
    const auto parent = CreatorServiceUtils::findEntityByID(objectRepository_, parentObjectId);
    const auto child = CreatorServiceUtils::findEntityByID(objectRepository_, childObjectId);

    return parent.has_value() && child.has_value() && !parent->deletedAt.has_value() && !child->deletedAt.has_value();
}

std::vector<ObjectRelation> ObjectRelationService::getChildRelations(std::int64_t parentObjectId) const
{
    if (!CreatorServiceUtils::findEntityByID(objectRepository_, parentObjectId).has_value())
        return {};

    const auto relations = repository_.findByParentObjectId(parentObjectId);

    std::vector<ObjectRelation> result;

    for (const auto &relation : relations)
    {
        if (areObjectsActive(relation.parentObjectId, relation.childObjectId))
            result.push_back(relation);
    }

    return result;
}

std::vector<ObjectRelation> ObjectRelationService::getParentRelations(std::int64_t childObjectId) const
{
    if (!CreatorServiceUtils::findEntityByID(objectRepository_, childObjectId).has_value())
        return {};

    const auto relations = repository_.findByChildObjectId(childObjectId);

    std::vector<ObjectRelation> result;

    for (const auto &relation : relations)
    {
        if (areObjectsActive(relation.parentObjectId, relation.childObjectId))
            result.push_back(relation);
    }

    return result;
}

std::optional<ObjectRelation> ObjectRelationService::getRelationByID(std::int64_t id) const
{
    const auto relation = CreatorServiceUtils::findEntityByID(repository_, id);

    if (!relation.has_value())
        return std::nullopt;

    if (!areObjectsActive(relation->parentObjectId, relation->childObjectId))
        return std::nullopt;

    return relation;
}

bool ObjectRelationService::isRoleValid(ObjectRelationRole role) const
{
    switch (role)
    {
        case ObjectRelationRole::Main:
        case ObjectRelationRole::Addition:
        case ObjectRelationRole::Support:
        return true;
    }

    return false;
}

bool ObjectRelationService::wouldCreateCycle(std::int64_t parentObjectId, std::int64_t childObjectId) const
{
    std::vector<std::int64_t> pending;
    std::unordered_set<std::int64_t> visited;

    pending.push_back(childObjectId);

    while (!pending.empty())
    {
        const std::int64_t currentObjectId = pending.back();
        pending.pop_back();

        if (currentObjectId == parentObjectId)
            return true;

        if (!visited.insert(currentObjectId).second)
            continue;

        const auto relations = repository_.findByParentObjectId(currentObjectId);

        for (const auto &relation : relations)
        {
            if (!areObjectsActive(relation.parentObjectId, relation.childObjectId))
                continue;

            pending.push_back(relation.childObjectId);
        }
    }

    return false;
}

ObjectRelation ObjectRelationService::createRelation(std::int64_t parentObjectId, std::int64_t childObjectId, ObjectRelationRole role)
{
    if (parentObjectId <= 0 || childObjectId <= 0)
        throw std::invalid_argument("Invalid object ID.");

    if (parentObjectId == childObjectId)
        throw std::invalid_argument("An object cannot be related to itself.");

    if (!areObjectsActive(parentObjectId, childObjectId))
        throw std::invalid_argument("Both objects must exist and be active.");

    if (!isRoleValid(role))
        throw std::invalid_argument("Invalid object relation role.");

    const auto existingRelations = repository_.findByParentObjectId(parentObjectId);

    const auto duplicate = std::find_if(
        existingRelations.begin(),
        existingRelations.end(),
        [childObjectId](const ObjectRelation &relation)
        {
            return relation.childObjectId == childObjectId;
        }
    );

    if (duplicate != existingRelations.end())
        throw std::invalid_argument("The relation already exists.");

    if (wouldCreateCycle(parentObjectId, childObjectId))
        throw std::invalid_argument("The relation would create a cycle.");

    ObjectRelation relation;

    relation.parentObjectId = parentObjectId;
    relation.childObjectId = childObjectId;
    relation.role = role;
    relation.uuid = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
    relation.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs).toStdString();
    relation.updatedAt = relation.createdAt;

    return CreatorServiceUtils::createAndLoadEntity(repository_, "Created object relation could not be loaded.", relation);
}

bool ObjectRelationService::updateRelationRole(std::int64_t id, ObjectRelationRole role)
{
    if (!isRoleValid(role))
        throw std::invalid_argument("Invalid object relation role.");

    const auto existingRelation = getRelationByID(id);

    if (!existingRelation.has_value())
        return false;

    ObjectRelation relation = existingRelation.value();

    relation.role = role;
    relation.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs).toStdString();

    return repository_.update(relation);
}

bool ObjectRelationService::deleteRelation(std::int64_t id)
{
    if (!repository_.findById(id).has_value())
        return false;

    return repository_.remove(id);
}