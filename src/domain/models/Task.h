#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

enum class TaskParentType
{
    Project,
    Content
};

enum class TaskStatus
{
    Todo,
    InProgress,
    Done,
    Cancelled
};

std::string taskParentTypeToString(TaskParentType type);
std::optional<TaskParentType> taskParentTypeFromString(std::string_view value);

std::string taskStatusToString(TaskStatus status);
std::optional<TaskStatus> taskStatusFromString(std::string_view value);

struct Task
{
    std::int64_t id = 0;

    std::int64_t parentId = 0;
    TaskParentType parentType = TaskParentType::Project;
    std::optional<std::int64_t> parentTaskId;

    std::string name;
    std::string description;
    std::string deadlineAt;

    TaskStatus status = TaskStatus::Todo;

    int priority = 0;

    std::string createdAt;
    std::string updatedAt;
};