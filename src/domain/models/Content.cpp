#include "Content.h"

std::string contentRoleToString(ContentRole role)
{
    switch (role)
    {
    case ContentRole::Main:
        return "main";
    case ContentRole::Additional:
        return "additional";
    }

    return {};
}

std::optional<ContentRole> contentRoleFromString(std::string_view value)
{
    if (value == "main")
    {
        return ContentRole::Main;
    }

    if (value == "additional")
    {
        return ContentRole::Additional;
    }

    return std::nullopt;
}

std::string contentStatusToString(ContentStatus status)
{
    switch (status)
    {
    case ContentStatus::Draft:
        return "draft";
    case ContentStatus::InProgress:
        return "in_progress";
    case ContentStatus::Ready:
        return "ready";
    case ContentStatus::Archived:
        return "archived";
    }

    return {};
}

std::optional<ContentStatus> contentStatusFromString(std::string_view value)
{
    if (value == "draft")
    {
        return ContentStatus::Draft;
    }

    if (value == "in_progress")
    {
        return ContentStatus::InProgress;
    }

    if (value == "ready")
    {
        return ContentStatus::Ready;
    }

    if (value == "archived")
    {
        return ContentStatus::Archived;
    }

    return std::nullopt;
}