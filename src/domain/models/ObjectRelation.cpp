#include "ObjectRelation.h"

#include <stdexcept>

std::string objectRelationRoleToString(ObjectRelationRole role)
{
    switch (role)
    {
    case ObjectRelationRole::Main:
        return "main";

    case ObjectRelationRole::Addition:
        return "addition";

    case ObjectRelationRole::Support:
        return "support";
    }

    throw std::logic_error("Unknown ObjectRelationRole.");
}

std::optional<ObjectRelationRole> objectRelationRoleFromString(std::string_view value)
{
    if (value == "main")
        return ObjectRelationRole::Main;

    if (value == "addition")
        return ObjectRelationRole::Addition;

    if (value == "support")
        return ObjectRelationRole::Support;

    return std::nullopt;
}