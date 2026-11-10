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
    explicit ContentService(IContentRepository &repository, IProjectRepository &projectRepository, IContentTypeRepository &contentTypeRepository);

    std::vector<Content> getAllContents() const;
    std::optional<Content> getContentByID(std::int64_t id) const;
    std::vector<Content> getProjectContents(std::int64_t projectId) const;

    Content createAdditionalContent(const Content &sourceContent);
    bool updateContent(const Content &sourceContent);
    bool deleteContent(std::int64_t id) const;

private:
    void setContentFields(Content &content, const Content &source) const;
    
    IContentRepository &repository_;
    IProjectRepository &projectRepository_;
    IContentTypeRepository &contentTypeRepository_;
};