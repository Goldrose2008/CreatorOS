#include "Content.h"

std::string contentRoleToString(ContentRole role)
{
    switch (role)
    {
    case ContentRole::Main:
        return "main";
    case ContentRole::Additional:
        return "additional";
    }

    return {};
}

std::optional<ContentRole> contentRoleFromString(std::string_view value)
{
    if (value == "main")
    {
        return ContentRole::Main;
    }

    if (value == "additional")
    {
        return ContentRole::Additional;
    }

    return std::nullopt;
}