#pragma once

#include <array>
#include <string_view>

namespace CreatorObjectTypeDefaults
{
    inline constexpr std::array<std::string_view, 7> Names
    {
        "Project",
        "Video",
        "Shorts",
        "Article",
        "Post",
        "Game",
        "Book"
    };
}