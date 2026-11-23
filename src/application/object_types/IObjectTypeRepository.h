#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "../../domain/models/ObjectType.h"

class IObjectTypeRepository
{
public:
    virtual ~IObjectTypeRepository() = default;

    virtual std::vector<ObjectType> findAll() const = 0;
    virtual std::optional<ObjectType> findById(std::int64_t id) const = 0;

    virtual bool existsByName(const std::string &name, const std::optional<std::int64_t> &excludeId = std::nullopt) const = 0;

    virtual std::int64_t create(const ObjectType &objectType) = 0;
    virtual bool update(const ObjectType &objectType) = 0;
    virtual bool remove(std::int64_t id) = 0;
};