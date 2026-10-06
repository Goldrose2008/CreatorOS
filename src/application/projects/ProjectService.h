#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "IProjectRepository.h"

class IContentTypeRepository;

class ProjectService
{
public:
    explicit ProjectService(
    IProjectRepository &repository,
    IContentTypeRepository &contentTypeRepository
);

    std::vector<Project> getProjects() const;
    std::optional<Project> getProject(std::int64_t id) const;

    Project createProject(
        const std::string &name,
        const std::string &description,
        const std::string &plannedReleaseAt,
        std::int64_t mainContentTypeId,
        const std::string &mainContentName
    );

    bool updateProject(
        std::int64_t id,
        const std::string &name,
        const std::string &description,
        const std::string &plannedReleaseAt
    );

    bool deleteProject(std::int64_t id) const;

private:
    IProjectRepository &repository_;
    IContentTypeRepository &contentTypeRepository_;
};