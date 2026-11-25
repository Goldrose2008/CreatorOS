#include "ObjectService.h"

#include <QDate>
#include <QDateTime>
#include <QUuid>

#include <stdexcept>

#include "IObjectRepository.h"
#include "../common/ServiceUtils.h"
#include "../common/StringUtils.h"
#include "../object_types/IObjectTypeRepository.h"

ObjectService::ObjectService(IObjectRepository &repository, IObjectTypeRepository &objectTypeRepository) : repository_(repository), objectTypeRepository_(objectTypeRepository)
{
}

std::vector<Object> ObjectService::getAllActiveObjects() const
{
    return repository_.findAllActive();
}

std::optional<Object> ObjectService::getObjectByID(std::int64_t id) const
{
    const auto object = CreatorServiceUtils::findEntityByID(repository_, id);

    if (!object.has_value() || object->deletedAt.has_value())
        return std::nullopt;

    return object;
}

bool ObjectService::isObjectNameValid(const std::string &name, const std::optional<std::int64_t> &excludeId) const
{
    const std::string normalizedName = CreatorStringUtils::trim(name);

    return !normalizedName.empty() && !repository_.existsActiveByName(normalizedName, excludeId);
}

void ObjectService::setObjectFields(Object &object, const Object &source) const
{
    if (!source.objectTypeId.has_value())
        throw std::invalid_argument("Object type is required.");

    if (!CreatorServiceUtils::findEntityByID(objectTypeRepository_, source.objectTypeId.value()).has_value())
        throw std::invalid_argument("Object type was not found.");

    const std::string normalizedName = CreatorStringUtils::trim(source.name);

    if (normalizedName.empty())
        throw std::invalid_argument("Object name cannot be empty.");

    if (!isObjectNameValid(normalizedName, object.id > 0 ? std::optional<std::int64_t>(object.id) : std::nullopt))
        throw std::invalid_argument("Object name already exists.");

    if (source.description.has_value())
    {
        if (QString::fromUtf8(source.description->c_str()).size() > 10000)
            throw std::invalid_argument("Object description cannot exceed 10000 characters.");

        object.description = source.description;
    }
    else
    {
        object.description = std::nullopt;
    }

    if (source.deadlineAt.has_value())
    {
        const QDate deadline = QDate::fromString(QString::fromStdString(source.deadlineAt.value()), Qt::ISODate        );

        if (!deadline.isValid() || deadline.toString(Qt::ISODate).toStdString() != source.deadlineAt.value())
            throw std::invalid_argument("Object deadline must be a valid date in YYYY-MM-DD format.");

        object.deadlineAt = source.deadlineAt;
    }
    else
    {
        object.deadlineAt = std::nullopt;
    }

    object.objectTypeId = source.objectTypeId;
    object.name = normalizedName;
    object.updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs).toStdString();
}

Object ObjectService::createObject(const Object &sourceObject)
{
    Object object;

    setObjectFields(object, sourceObject);

    object.uuid = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
    object.createdAt = object.updatedAt;

    return CreatorServiceUtils::createAndLoadEntity(repository_, "Created object could not be loaded.", object    );
}

bool ObjectService::updateObject(const Object &sourceObject)
{
    const auto existingObject = CreatorServiceUtils::findEntityByID(repository_, sourceObject.id);

    if (!existingObject.has_value() || existingObject->deletedAt.has_value())
        return false;

    Object object = existingObject.value();

    setObjectFields(object, sourceObject);

    return repository_.update(object);
}