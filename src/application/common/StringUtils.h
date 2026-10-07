#pragma once

#include <cctype>
#include <string>
#include <string_view>

namespace CreatorStringUtils
{
    inline std::string trim(std::string_view value)
    {
        std::size_t first = 0;

        while (first < value.size() && std::isspace(static_cast<unsigned char>(value[first])))
        {
            ++first;
        }

        std::size_t last = value.size();

        while (last > first && std::isspace(static_cast<unsigned char>(value[last - 1])))
        {
            --last;
        }

        return std::string(value.substr(first, last - first));
    }
}