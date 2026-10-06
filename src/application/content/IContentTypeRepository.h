#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "../../domain/models/ContentType.h"

class IContentTypeRepository
{
public:
    virtual ~IContentTypeRepository() = default;

    virtual std::vector<ContentType> findAll() const = 0;
    virtual std::optional<ContentType> findById(std::int64_t id) const = 0;
};