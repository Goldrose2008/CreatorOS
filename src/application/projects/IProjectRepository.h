#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

#include "../../domain/models/Project.h"

class IProjectRepository
{
public:
    virtual ~IProjectRepository() = default;
    virtual std::vector<Project> findAll() const = 0;
    virtual std::optional<Project> findById(std::int64_t id) const = 0;
    virtual std::int64_t create(const Project &project) = 0;
    virtual bool update(const Project &project) = 0;
    virtual bool remove(std::int64_t id) = 0;
};