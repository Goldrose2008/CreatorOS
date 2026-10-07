#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "../../domain/models/Task.h"

class ITaskRepository
{
public:
    virtual ~ITaskRepository() = default;
    virtual std::vector<Task> findAll() const = 0;
    virtual std::vector<Task> findByParent(TaskParentType parentType, std::int64_t parentId, const std::optional<std::int64_t> &parentTaskId) const = 0;
    virtual std::optional<Task> findById(std::int64_t id) const = 0;
    virtual std::int64_t create(const Task &task) = 0;
    virtual bool update(const Task &task) = 0;
    virtual bool remove(std::int64_t id) = 0;
};