#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "../../domain/models/Platform.h"

class IPlatformRepository
{
public:
    virtual ~IPlatformRepository() = default;

    virtual std::vector<Platform> findAll() const = 0;
    virtual std::optional<Platform> findById(std::int64_t id) const = 0;
    virtual bool hasAccounts(std::int64_t id) const = 0;

    virtual std::int64_t create(const Platform &platform) = 0;
    virtual bool update(const Platform &platform) = 0;
    virtual bool remove(std::int64_t id) = 0;
};