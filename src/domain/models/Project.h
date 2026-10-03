#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

enum class ProjectStatus
{
    Draft,
    Active,
    Archived
};

std::string projectStatusToString(ProjectStatus status);
std::optional<ProjectStatus> projectStatusFromString(std::string_view value);

struct Project
{
    std::int64_t id = 0;
    std::string name;
    std::string description;
    std::optional<std::int64_t> ownerId;
    std::string plannedReleaseAt;
    ProjectStatus status = ProjectStatus::Draft;
    int progress = 0;
    std::string createdAt;
    std::string updatedAt;
};