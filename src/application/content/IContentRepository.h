#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "../../domain/models/Content.h"

class IContentRepository
{
public:
    virtual ~IContentRepository() = default;

    virtual std::vector<Content> findByProjectId(std::int64_t projectId) const = 0;
    virtual std::optional<Content> findById(std::int64_t id) const = 0;

    virtual std::int64_t create(const Content &content) = 0;
    virtual bool update(const Content &content) = 0;
    virtual bool remove(std::int64_t id) = 0;
};