#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "../../domain/models/Object.h"

class IObjectRepository;
class IObjectTypeRepository;

class ObjectService
{
public:
    ObjectService(IObjectRepository &repository, IObjectTypeRepository &objectTypeRepository);

    std::vector<Object> getAllActiveObjects() const;
    std::optional<Object> getObjectByID(std::int64_t id) const;

    bool isObjectNameValid(const std::string &name, const std::optional<std::int64_t> &excludeId = std::nullopt) const;

    Object createObject(const Object &sourceObject);
    bool updateObject(const Object &sourceObject);

private:
    void setObjectFields(Object &object, const Object &source) const;

    IObjectRepository &repository_;
    IObjectTypeRepository &objectTypeRepository_;
};