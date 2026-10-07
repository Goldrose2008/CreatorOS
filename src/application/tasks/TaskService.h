#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "../../domain/models/Task.h"

class IContentRepository;
class IProjectRepository;
class ITaskRepository;

class TaskService
{
public:
    explicit TaskService(ITaskRepository &repository, IProjectRepository &projectRepository, IContentRepository &contentRepository);

    std::vector<Task> getTasks() const;
    std::vector<Task> getTasksForParent(TaskParentType parentType, std::int64_t parentId, const std::optional<std::int64_t> &parentTaskId = std::nullopt) const;
    std::optional<Task> getTask(std::int64_t id) const;
    
    Task createTask(TaskParentType parentType, std::int64_t parentId, const std::optional<std::int64_t> &parentTaskId, const std::string &name, const std::string &description, const std::string &deadlineAt);

    bool updateTask(std::int64_t id, const std::string &name, const std::string &description, const std::string &deadlineAt);
    bool deleteTask(std::int64_t id) const;

private:
    void validateParent(TaskParentType parentType, std::int64_t parentId, const std::optional<std::int64_t> &parentTaskId) const;

    ITaskRepository &repository_;
    IProjectRepository &projectRepository_;
    IContentRepository &contentRepository_;
};