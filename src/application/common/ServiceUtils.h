#pragma once

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>

namespace CreatorServiceUtils
{
    template <typename Repository>
    auto findEntityByID(const Repository &repository, std::int64_t id) -> decltype(repository.findById(id))
    {
        if (id <= 0)
            return std::nullopt;

        return repository.findById(id);
    }

    template <typename Repository, typename... Args>
    auto createAndLoadEntity(Repository &repository, const std::string &errorMessage, Args&&... args)
    {
        const std::int64_t id = repository.create(std::forward<Args>(args)...);
        const auto entity = findEntityByID(repository, id);

        if (!entity.has_value())
            throw std::runtime_error(errorMessage);

        return entity.value();
    }
}