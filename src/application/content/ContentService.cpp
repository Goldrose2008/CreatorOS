#include "ContentService.h"

#include <stdexcept>

#include "IContentRepository.h"
#include "IContentTypeRepository.h"
#include "../common/ServiceUtils.h"
#include "../common/StringUtils.h"
#include "../projects/IProjectRepository.h"

ContentService::ContentService(IContentRepository &repository, IProjectRepository &projectRepository, IContentTypeRepository &contentTypeRepository)
    : repository_(repository), projectRepository_(projectRepository), contentTypeRepository_(contentTypeRepository)
{
}

std::vector<Content> ContentService::getAllContents() const
{
    return repository_.findAll();
}

std::vector<Content> ContentService::getProjectContents(std::int64_t projectId) const
{
    if (projectId <= 0)
        return {};

    return repository_.findByProjectId(projectId);
}

std::optional<Content> ContentService::getContentByID(std::int64_t id) const
{
    return CreatorServiceUtils::findEntityByID(repository_, id);
}

Content ContentService::createAdditionalContent(
    std::int64_t projectId,
    std::int64_t contentTypeId,
    const std::string &name,
    const std::string &description,
    int priority
)
{
    if (projectId <= 0)
        throw std::invalid_argument("Project is required.");

    if (!CreatorServiceUtils::findEntityByID(projectRepository_,projectId).has_value())
        throw std::invalid_argument("Project was not found.");

    if (contentTypeId <= 0)
        throw std::invalid_argument("Content type is required.");

    if (!CreatorServiceUtils::findEntityByID(contentTypeRepository_, contentTypeId).has_value())
        throw std::invalid_argument("Content type was not found.");

    const std::string normalizedName = CreatorStringUtils::trim(name);

    if (normalizedName.empty())
        throw std::invalid_argument("Content name cannot be empty.");

    Content content;

    content.projectId = projectId;
    content.contentTypeId = contentTypeId;
    content.role = ContentRole::Additional;
    content.name = normalizedName;
    content.description = CreatorStringUtils::trim(description);
    content.priority = priority;
    content.status = ContentStatus::Draft;
    content.progress = 0;

    return CreatorServiceUtils::createAndLoadEntity(
        repository_,
        "Created content could not be loaded.",
        content);
}

bool ContentService::updateContent(
    std::int64_t id,
    std::int64_t contentTypeId,
    const std::string &name,
    const std::string &description,
    int priority
)
{
    const auto existingContent = CreatorServiceUtils::findEntityByID(repository_, id);

    if (!existingContent.has_value())
        return false;

    if (contentTypeId <= 0)
        throw std::invalid_argument("Content type is required.");

    if (!CreatorServiceUtils::findEntityByID(contentTypeRepository_, contentTypeId).has_value())
        throw std::invalid_argument("Content type was not found.");

    const std::string normalizedName = CreatorStringUtils::trim(name);

    if (normalizedName.empty())
        throw std::invalid_argument("Content name cannot be empty.");

    Content content = existingContent.value();

    content.contentTypeId = contentTypeId;
    content.name = normalizedName;
    content.description = CreatorStringUtils::trim(description);
    content.priority = priority;

    return repository_.update(content);
}

bool ContentService::deleteContent(std::int64_t id) const
{
    const auto existingContent = CreatorServiceUtils::findEntityByID(repository_, id);

    if (!existingContent.has_value())
        return false;

    if (existingContent->role == ContentRole::Main)
        throw std::invalid_argument("Main content cannot be deleted from its project.");

    return repository_.remove(id);
}