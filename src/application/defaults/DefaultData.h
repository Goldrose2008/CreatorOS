#pragma once

#include <array>
#include <string_view>

namespace CreatorDefaultData
{
    inline constexpr std::array<std::string_view, 7> ObjectTypeNames
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