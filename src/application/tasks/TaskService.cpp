#include "TaskService.h"

#include <stdexcept>

#include "ITaskRepository.h"
#include "../common/ServiceUtils.h"
#include "../common/StringUtils.h"
#include "../content/IContentRepository.h"
#include "../projects/IProjectRepository.h"

TaskService::TaskService(ITaskRepository &repository, IProjectRepository &projectRepository, IContentRepository &contentRepository)
    : repository_(repository), projectRepository_(projectRepository), contentRepository_(contentRepository)
{
}

std::vector<Task> TaskService::getAllTasks() const
{
    return repository_.findAll();
}

std::vector<Task> TaskService::getTasksForParent(TaskParentType parentType, std::int64_t parentId, const std::optional<std::int64_t> &parentTaskId) const
{
    if (parentId <= 0)
        return {};

    return repository_.findByParent(parentType, parentId, parentTaskId);
}

std::optional<Task> TaskService::getTaskByID(std::int64_t id) const
{
    return CreatorServiceUtils::findEntityByID(repository_, id);
}

void TaskService::validateParent(TaskParentType parentType, std::int64_t parentId, const std::optional<std::int64_t> &parentTaskId) const
{
    if (parentId <= 0)
        throw std::invalid_argument("Task parent is required.");

    switch (parentType)
    {
    case TaskParentType::Project:
        if (!projectRepository_.findById(parentId).has_value())
            throw std::invalid_argument("Task parent project was not found.");

        break;

    case TaskParentType::Content:
        if (!contentRepository_.findById(parentId).has_value())
            throw std::invalid_argument("Task parent content was not found.");

        break;
    }

    if (!parentTaskId.has_value())
        return;

    if (parentTaskId.value() <= 0)
        throw std::invalid_argument("Parent task is invalid.");

    const auto parentTask = CreatorServiceUtils::findEntityByID(repository_, parentTaskId.value());

    if (!parentTask.has_value())
        throw std::invalid_argument("Parent task was not found.");

    if (parentTask->parentType != parentType || parentTask->parentId != parentId)
        throw std::invalid_argument("Parent task belongs to another parent context.");
}

Task TaskService::createTask(
    TaskParentType parentType,
    std::int64_t parentId,
    const std::optional<std::int64_t> &parentTaskId,
    const std::string &name,
    const std::string &description,
    const std::string &deadlineAt
)
{
    validateParent(parentType, parentId, parentTaskId);

    const std::string normalizedName = CreatorStringUtils::trim(name);

    if (normalizedName.empty())
        throw std::invalid_argument("Task name cannot be empty.");

    Task task;
    task.parentId = parentId;
    task.parentType = parentType;
    task.parentTaskId = parentTaskId;
    task.name = normalizedName;
    task.description = CreatorStringUtils::trim(description);
    task.deadlineAt = CreatorStringUtils::trim(deadlineAt);
    task.status = TaskStatus::Todo;
    task.priority = 0;

    return CreatorServiceUtils::createAndLoadEntity(
        repository_,
        "Created task could not be loaded.",
        task);
}

bool TaskService::updateTask(std::int64_t id, const std::string &name, const std::string &description, const std::string &deadlineAt)
{
    const auto existingTask = CreatorServiceUtils::findEntityByID(repository_, id);

    if (!existingTask.has_value())
        return false;

    const std::string normalizedName = CreatorStringUtils::trim(name);

    if (normalizedName.empty())
        throw std::invalid_argument("Task name cannot be empty.");

    Task task = existingTask.value();
    task.name = normalizedName;
    task.description = CreatorStringUtils::trim(description);
    task.deadlineAt = CreatorStringUtils::trim(deadlineAt);

    return repository_.update(task);
}

bool TaskService::deleteTask(std::int64_t id) const
{
    return repository_.remove(id);
}