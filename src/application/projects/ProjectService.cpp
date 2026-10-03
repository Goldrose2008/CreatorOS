#include "ProjectService.h"

#include <cctype>
#include <stdexcept>

namespace
{
    std::string trim(const std::string &value)
    {
        std::size_t first = 0;

        while (first < value.size() && std::isspace(static_cast<unsigned char>(value[first])))
        {
            ++first;
        }

        std::size_t last = value.size();

        while (last > first && std::isspace(static_cast<unsigned char>(value[last - 1])))
        {
            --last;
        }

        return value.substr(first, last - first);
    }
}

ProjectService::ProjectService(IProjectRepository &repository)
    : repository_(repository)
{
}

std::vector<Project> ProjectService::getProjects() const
{
    return repository_.findAll();
}

std::optional<Project> ProjectService::getProject(std::int64_t id) const
{
    return repository_.findById(id);
}

Project ProjectService::createProject(
    const std::string &name,
    const std::string &description,
    const std::string &plannedReleaseAt
)
{
    const std::string normalizedName = trim(name);
    const std::string normalizedDescription = trim(description);
    const std::string normalizedReleaseAt = trim(plannedReleaseAt);

    if (normalizedName.empty())
    {
        throw std::invalid_argument("Project name cannot be empty.");
    }

    if (normalizedReleaseAt.empty())
    {
        throw std::invalid_argument("Project planned release date cannot be empty.");
    }

    Project project;

    project.name = normalizedName;
    project.description = normalizedDescription;
    project.plannedReleaseAt = normalizedReleaseAt;
    project.status = ProjectStatus::Draft;
    project.progress = 0;

    const std::int64_t id = repository_.create(project);

    const auto createdProject = repository_.findById(id);

    if (!createdProject.has_value())
    {
        throw std::runtime_error("Created project could not be loaded.");
    }

    return createdProject.value();
}

bool ProjectService::updateProject(
    std::int64_t id,
    const std::string &name,
    const std::string &description,
    const std::string &plannedReleaseAt
)
{
    const auto existingProject = repository_.findById(id);

    if (!existingProject.has_value()){return false;}

    const std::string normalizedName = trim(name);
    const std::string normalizedDescription = trim(description);
    const std::string normalizedReleaseAt = trim(plannedReleaseAt);

    if (normalizedName.empty())
    {
        throw std::invalid_argument("Project name cannot be empty.");
    }

    if (normalizedReleaseAt.empty())
    {
        throw std::invalid_argument("Project planned release date cannot be empty.");
    }

    Project project = existingProject.value();

    project.name = normalizedName;
    project.description = normalizedDescription;
    project.plannedReleaseAt = normalizedReleaseAt;

    return repository_.update(project);
}

bool ProjectService::deleteProject(std::int64_t id) const
{
    return repository_.remove(id);
}