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

std::optional<Content> ContentService::getContentByID(std::int64_t id) const
{
    return CreatorServiceUtils::findEntityByID(repository_, id);
}

std::vector<ContentType> ContentService::getContentTypes() const
{
    return contentTypeRepository_.findAll();
}

std::vector<Content> ContentService::getProjectContents(std::int64_t projectId) const
{
    if (projectId <= 0)
        return {};

    return repository_.findByProjectId(projectId);
}

void ContentService::setContentFields(Content &content, const Content &source) const
{
    if (source.contentTypeId <= 0)
        throw std::invalid_argument("Content type is required.");

    if (!CreatorServiceUtils::findEntityByID(contentTypeRepository_, source.contentTypeId).has_value())
        throw std::invalid_argument("Content type was not found.");

    if (CreatorStringUtils::isBlank(source.name))
        throw std::invalid_argument("Content name cannot be empty.");

    content.contentTypeId = source.contentTypeId;
    content.name = CreatorStringUtils::trim(source.name);
    content.description = CreatorStringUtils::trim(source.description);
    content.priority = source.priority;
}

Content ContentService::createAdditionalContent(const Content &sourceContent)
{
    if (sourceContent.projectId <= 0)
        throw std::invalid_argument("Project is required.");

    if (!CreatorServiceUtils::findEntityByID(projectRepository_, sourceContent.projectId).has_value())
        throw std::invalid_argument("Project was not found.");

    Content content;
    content.projectId = sourceContent.projectId;
    content.role = ContentRole::Additional;
    content.status = ContentStatus::Draft;
    content.progress = 0;

    setContentFields(content, sourceContent);

    return CreatorServiceUtils::createAndLoadEntity(
        repository_,
        "Created content could not be loaded.",
        content);
}

bool ContentService::updateContent(const Content &sourceContent)
{
    const auto existingContent = CreatorServiceUtils::findEntityByID(repository_, sourceContent.id);

    if (!existingContent.has_value())
        return false;

    Content content = existingContent.value();
    setContentFields(content, sourceContent);

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