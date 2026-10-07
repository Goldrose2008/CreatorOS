#include "ContentService.h"

#include <stdexcept>

#include "IContentRepository.h"
#include "IContentTypeRepository.h"
#include "../projects/IProjectRepository.h"
#include "../common/StringUtils.h"

ContentService::ContentService(
    IContentRepository &repository,
    IProjectRepository &projectRepository,
    IContentTypeRepository &contentTypeRepository
)
    : repository_(repository),
      projectRepository_(projectRepository),
      contentTypeRepository_(contentTypeRepository)
{
}

std::vector<Content> ContentService::getProjectContents(std::int64_t projectId) const
{
    if (projectId <= 0)
    {
        return {};
    }

    return repository_.findByProjectId(projectId);
}

std::optional<Content> ContentService::getContent(std::int64_t id) const
{
    if (id <= 0){return std::nullopt;}
    return repository_.findById(id);
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
    {
        throw std::invalid_argument("Project is required.");
    }

    if (!projectRepository_.findById(projectId).has_value())
    {
        throw std::invalid_argument("Project was not found.");
    }

    if (contentTypeId <= 0)
    {
        throw std::invalid_argument("Content type is required.");
    }

    if (!contentTypeRepository_.findById(contentTypeId).has_value())
    {
        throw std::invalid_argument("Content type was not found.");
    }

    const std::string normalizedName = CreatorStringUtils::trim(name);

    if (normalizedName.empty())
    {
        throw std::invalid_argument("Content name cannot be empty.");
    }

    Content content;

    content.projectId = projectId;
    content.contentTypeId = contentTypeId;
    content.role = ContentRole::Additional;
    content.name = normalizedName;
    content.description = CreatorStringUtils::trim(description);
    content.priority = priority;
    content.status = ContentStatus::Draft;
    content.progress = 0;

    const std::int64_t id = repository_.create(content);

    const auto createdContent = repository_.findById(id);

    if (!createdContent.has_value())
    {
        throw std::runtime_error("Created content could not be loaded.");
    }

    return createdContent.value();
}

bool ContentService::updateContent(
    std::int64_t id,
    std::int64_t contentTypeId,
    const std::string &name,
    const std::string &description,
    int priority
)
{
    const auto existingContent = repository_.findById(id);

    if (!existingContent.has_value())
    {
        return false;
    }

    if (contentTypeId <= 0)
    {
        throw std::invalid_argument("Content type is required.");
    }

    if (!contentTypeRepository_.findById(contentTypeId).has_value())
    {
        throw std::invalid_argument("Content type was not found.");
    }

    const std::string normalizedName = CreatorStringUtils::trim(name);

    if (normalizedName.empty())
    {
        throw std::invalid_argument("Content name cannot be empty.");
    }

    Content content = existingContent.value();

    content.contentTypeId = contentTypeId;
    content.name = normalizedName;
    content.description = CreatorStringUtils::trim(description);
    content.priority = priority;

    return repository_.update(content);
}

bool ContentService::deleteContent(std::int64_t id) const
{
    const auto existingContent = repository_.findById(id);

    if (!existingContent.has_value()){return false;}

    if (existingContent->role == ContentRole::Main)
    {
        throw std::invalid_argument("Main content cannot be deleted from its project.");
    }

    return repository_.remove(id);
}