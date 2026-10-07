#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

enum class ContentRole
{
    Main,
    Additional
};

enum class ContentStatus
{
    Draft,
    InProgress,
    Ready,
    Archived
};

std::string contentRoleToString(ContentRole role);
std::optional<ContentRole> contentRoleFromString(std::string_view value);

std::string contentStatusToString(ContentStatus status);
std::optional<ContentStatus> contentStatusFromString(std::string_view value);

struct Content
{
    std::int64_t id = 0;
    std::int64_t projectId = 0;
    std::int64_t contentTypeId = 0;
    ContentRole role = ContentRole::Main;

    std::string name;
    std::string description;
    int priority = 0;
    std::string productionDeadlineAt;

    ContentStatus status = ContentStatus::Draft;
    int progress = 0;

    std::string createdAt;
    std::string updatedAt;
};