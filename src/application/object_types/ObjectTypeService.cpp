#include "ObjectTypeService.h"

#include <QDateTime>
#include <QUuid>

#include <stdexcept>

#include "IObjectTypeRepository.h"
#include "../objects/IObjectRepository.h"
#include "../common/ServiceUtils.h"
#include "../common/StringUtils.h"

ObjectTypeService::ObjectTypeService(IObjectTypeRepository &repository, IObjectRepository &objectRepository)
    : repository_(repository), objectRepository_(objectRepository)
{
}

std::vector<ObjectType> ObjectTypeService::getAllObjectTypes() const
{
    return repository_.findAll();
}

std::optional<ObjectType> ObjectTypeService::getObjectTypeByID(std::int64_t id) const
{
    return CreatorServiceUtils::findEntityByID(repository_, id);
}

bool ObjectTypeService::isObjectTypeNameValid(const std::string &name, const std::optional<std::int64_t> &excludeId) const
{
    const std::string normalizedName = CreatorStringUtils::trim(name);

    return !normalizedName.empty() && !repository_.existsByName(normalizedName, excludeId);
}

void ObjectTypeService::setObjectTypeFields(ObjectType &objectType, const ObjectType &source) const
{
    const std::string normalizedName = CreatorStringUtils::trim(source.name);

    if (normalizedName.empty())
        throw std::invalid_argument("Object type name cannot be empty.");

    if (!isObjectTypeNameValid(normalizedName, objectType.id > 0 ? std::optional<std::int64_t>(objectType.id) : std::nullopt))
    {
        throw std::invalid_argument("Object type name already exists.");
    }

    objectType.name = normalizedName;

    if (source.description.has_value())
    {
        const std::string description = CreatorStringUtils::trim(*source.description);
        objectType.description = description.empty() ? std::nullopt : std::optional<std::string>(description);
    }
    else
    {
        objectType.description = std::nullopt;
    }

    objectType.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs).toStdString();
}

ObjectType ObjectTypeService::createObjectType(const ObjectType &sourceObjectType)
{
    ObjectType objectType;

    setObjectTypeFields(objectType, sourceObjectType);

    objectType.uuid = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
    objectType.createdAt = objectType.updatedAt;

    return CreatorServiceUtils::createAndLoadEntity(repository_, "Created object type could not be loaded.", objectType);
}

bool ObjectTypeService::updateObjectType(const ObjectType &sourceObjectType)
{
    const auto existingObjectType = CreatorServiceUtils::findEntityByID(repository_, sourceObjectType.id);

    if (!existingObjectType.has_value())
        return false;

    ObjectType objectType = existingObjectType.value();
    setObjectTypeFields(objectType, sourceObjectType);

    return repository_.update(objectType);
}

bool ObjectTypeService::deleteObjectType(std::int64_t id) const
{
    if (id <= 0)
        return false;

    const auto objectType = repository_.findById(id);

    if (!objectType.has_value())
        return false;

    if (!objectRepository_.findActiveByObjectTypeId(id).empty())
        throw std::invalid_argument("Object type is used by active objects. Reassign them before deletion.");

    return repository_.remove(id);
}

void ObjectTypeService::createDefaultObjectTypes()
{
    for (const std::string_view name : DefaultObjectTypeNames)
    {
        const std::string objectTypeName(name);

        if (repository_.existsByName(objectTypeName))
            continue;

        ObjectType objectType;
        objectType.name = objectTypeName;

        createObjectType(objectType);
    }
}