#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "../../domain/models/Content.h"

class IContentRepository;
class IContentTypeRepository;
class IProjectRepository;

class ContentService
{
public:
    explicit ContentService(
        IContentRepository &repository,
        IProjectRepository &projectRepository,
        IContentTypeRepository &contentTypeRepository
    );

    std::vector<Content> getProjectContents(std::int64_t projectId) const;
    std::optional<Content> getContent(std::int64_t id) const;

    Content createAdditionalContent(
        std::int64_t projectId,
        std::int64_t contentTypeId,
        const std::string &name,
        const std::string &description,
        int priority
    );

    bool updateContent(
        std::int64_t id,
        std::int64_t contentTypeId,
        const std::string &name,
        const std::string &description,
        int priority
    );

    bool deleteContent(std::int64_t id) const;

private:
    IContentRepository &repository_;
    IProjectRepository &projectRepository_;
    IContentTypeRepository &contentTypeRepository_;
};