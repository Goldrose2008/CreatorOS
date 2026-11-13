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

    std::vector<Task> getAllTasks() const;
    std::optional<Task> getTaskByID(std::int64_t id) const;
    std::vector<Task> getTasksForParent(TaskParentType parentType, std::int64_t parentId, const std::optional<std::int64_t> &parentTaskId = std::nullopt) const;

    bool isTaskNameValid(const std::string &name) const;
    
    Task createTask(const Task &sourceTask);
    bool updateTask(const Task &sourceTask);
    bool deleteTask(std::int64_t id) const;

private:
    void validateParent(TaskParentType parentType, std::int64_t parentId, const std::optional<std::int64_t> &parentTaskId) const;
    void setTaskFields(Task &task, const Task &source) const;
    
    ITaskRepository &repository_;
    IProjectRepository &projectRepository_;
    IContentRepository &contentRepository_;
};