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

std::optional<Task> TaskService::getTaskByID(std::int64_t id) const
{
    return CreatorServiceUtils::findEntityByID(repository_, id);
}

std::vector<Task> TaskService::getTasksForParent(TaskParentType parentType, std::int64_t parentId, const std::optional<std::int64_t> &parentTaskId) const
{
    if (parentId <= 0)
        return {};

    return repository_.findByParent(parentType, parentId, parentTaskId);
}

bool TaskService::isTaskNameValid(const std::string &name) const
{
    return !CreatorStringUtils::isBlank(name);
}

void TaskService::validateParent(TaskParentType parentType, std::int64_t parentId, const std::optional<std::int64_t> &parentTaskId) const
{
    if (parentId <= 0)
        throw std::invalid_argument("Task parent is required.");

    switch (parentType)
    {
    case TaskParentType::Project:
        if (!CreatorServiceUtils::findEntityByID(projectRepository_, parentId).has_value())
            throw std::invalid_argument("Task parent project was not found.");

        break;

    case TaskParentType::Content:
        if (!CreatorServiceUtils::findEntityByID(contentRepository_, parentId).has_value())
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

void TaskService::setTaskFields(Task &task, const Task &source) const
{
    if (!isTaskNameValid(source.name))
        throw std::invalid_argument("Task name cannot be empty.");

    task.name = CreatorStringUtils::trim(source.name);
    task.description = CreatorStringUtils::trim(source.description);
    task.deadlineAt = CreatorStringUtils::trim(source.deadlineAt);
}

Task TaskService::createTask(const Task &sourceTask)
{
    validateParent(sourceTask.parentType, sourceTask.parentId, sourceTask.parentTaskId);

    Task task;
    task.parentId = sourceTask.parentId;
    task.parentType = sourceTask.parentType;
    task.parentTaskId = sourceTask.parentTaskId;
    task.status = TaskStatus::Todo;
    task.priority = 0;

    setTaskFields(task, sourceTask);

    return CreatorServiceUtils::createAndLoadEntity(
        repository_,
        "Created task could not be loaded.",
        task);
}

bool TaskService::updateTask(const Task &sourceTask)
{
    const auto existingTask = CreatorServiceUtils::findEntityByID(repository_, sourceTask.id);

    if (!existingTask.has_value())
        return false;

    Task task = existingTask.value();
    setTaskFields(task, sourceTask);

    return repository_.update(task);
}

bool TaskService::deleteTask(std::int64_t id) const
{
    return repository_.remove(id);
}