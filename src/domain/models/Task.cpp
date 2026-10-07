#include "Task.h"

std::string taskParentTypeToString(TaskParentType type)
{
    switch (type)
    {
    case TaskParentType::Project:
        return "project";
    case TaskParentType::Content:
        return "content";
    }

    return {};
}

std::optional<TaskParentType> taskParentTypeFromString(std::string_view value)
{
    if (value == "project")
    {
        return TaskParentType::Project;
    }

    if (value == "content")
    {
        return TaskParentType::Content;
    }

    return std::nullopt;
}

std::string taskStatusToString(TaskStatus status)
{
    switch (status)
    {
    case TaskStatus::Todo:
        return "todo";
    case TaskStatus::InProgress:
        return "in_progress";
    case TaskStatus::Done:
        return "done";
    case TaskStatus::Cancelled:
        return "cancelled";
    }

    return {};
}

std::optional<TaskStatus> taskStatusFromString(std::string_view value)
{
    if (value == "todo")
    {
        return TaskStatus::Todo;
    }

    if (value == "in_progress")
    {
        return TaskStatus::InProgress;
    }

    if (value == "done")
    {
        return TaskStatus::Done;
    }

    if (value == "cancelled")
    {
        return TaskStatus::Cancelled;
    }

    return std::nullopt;
}