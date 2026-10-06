#include "ProjectService.h"

#include <cctype>
#include <stdexcept>

#include "../content/IContentTypeRepository.h"

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

    void setEditableFields(
        Project &project,
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

        project.name = normalizedName;
        project.description = normalizedDescription;
        project.plannedReleaseAt = normalizedReleaseAt;
    }
}

ProjectService::ProjectService(
    IProjectRepository &repository,
    IContentTypeRepository &contentTypeRepository
)
    : repository_(repository),
      contentTypeRepository_(contentTypeRepository)
{
}

std::vector<Project> ProjectService::getProjects() const
{
    return repository_.findAll();
}

std::vector<ContentType> ProjectService::getContentTypes() const
{
    return contentTypeRepository_.findAll();
}

std::optional<Project> ProjectService::getProject(std::int64_t id) const
{
    return repository_.findById(id);
}

Project ProjectService::createProject(
    const std::string &name,
    const std::string &description,
    const std::string &plannedReleaseAt,
    std::int64_t mainContentTypeId,
    const std::string &mainContentName
)
{
    Project project;

    setEditableFields(project, name, description, plannedReleaseAt);

    const std::string normalizedContentName = trim(mainContentName);

    if (normalizedContentName.empty())
    {
        throw std::invalid_argument("Main content name cannot be empty.");
    }

    if (mainContentTypeId <= 0)
    {
        throw std::invalid_argument("Main content type is required.");
    }

    const auto mainContentType = contentTypeRepository_.findById(mainContentTypeId);
    if (!mainContentType.has_value())
    {
        throw std::invalid_argument("Main content type was not found.");
    }

    project.status = ProjectStatus::Draft;
    project.progress = 0;

    Content mainContent;

    mainContent.contentTypeId = mainContentTypeId;
    mainContent.role = ContentRole::Main;
    mainContent.name = normalizedContentName;

    const std::int64_t id = repository_.createWithMainContent(project, mainContent);

    const auto createdProject = repository_.findById(id);

    if (!createdProject.has_value())
    {
        throw std::runtime_error("Created project could not be loaded.");
    }

    return createdProject.value();
}

bool ProjectService::updateProject(std::int64_t id, const std::string &name, const std::string &description, const std::string &plannedReleaseAt)
{
    const auto existingProject = repository_.findById(id);

    if (!existingProject.has_value()){return false;}

    Project project = existingProject.value();

    setEditableFields(project, name, description, plannedReleaseAt);

    return repository_.update(project);
}

bool ProjectService::deleteProject(std::int64_t id) const
{
    return repository_.remove(id);
}