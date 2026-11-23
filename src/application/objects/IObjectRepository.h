#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "../../domain/models/Object.h"

class IObjectRepository
{
public:
    virtual ~IObjectRepository() = default;

    virtual std::vector<Object> findAllActive() const = 0;
    virtual std::vector<Object> findAllInTrash() const = 0;

    virtual std::optional<Object> findById(std::int64_t id) const = 0;
    virtual std::vector<Object> findActiveByObjectTypeId(std::int64_t objectTypeId) const = 0;

    virtual bool existsActiveByName(const std::string &name, const std::optional<std::int64_t> &excludeId = std::nullopt) const = 0;

    virtual std::int64_t create(const Object &object) = 0;
    virtual bool update(const Object &object) = 0;
    virtual bool removePermanently(std::int64_t id) = 0;
};