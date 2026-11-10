#pragma once

#include <cstdint>
#include <optional>

namespace CreatorServiceUtils
{
    template <typename Repository>
    auto findEntityByID(const Repository &repository, std::int64_t id) -> decltype(repository.findById(id))
    {
        if (id <= 0)
            return std::nullopt;

        return repository.findById(id);
    }
}