#include "Project.h"

#include <stdexcept>

std::string projectStatusToString(ProjectStatus status)
{
    switch (status)
    {
    case ProjectStatus::Draft:
        return "draft";

    case ProjectStatus::Active:
        return "active";

    case ProjectStatus::Archived:
        return "archived";
    }

    throw std::logic_error("Unknown ProjectStatus.");
}

std::optional<ProjectStatus> projectStatusFromString(std::string_view value)
{
    if (value == "draft")
    {
        return ProjectStatus::Draft;
    }

    if (value == "active")
    {
        return ProjectStatus::Active;
    }

    if (value == "archived")
    {
        return ProjectStatus::Archived;
    }

    return std::nullopt;
}