#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "../../domain/models/ObjectType.h"

class IObjectTypeRepository;
class IObjectRepository;

class ObjectTypeService
{
public:
    ObjectTypeService(IObjectTypeRepository &repository, IObjectRepository &objectRepository);

    std::vector<ObjectType> getAllObjectTypes() const;
    std::optional<ObjectType> getObjectTypeByID(std::int64_t id) const;

    bool isObjectTypeNameValid(const std::string &name, const std::optional<std::int64_t> &excludeId = std::nullopt) const;

    ObjectType createObjectType(const ObjectType &sourceObjectType);
    bool updateObjectType(const ObjectType &sourceObjectType);
    bool deleteObjectType(std::int64_t id) const;

    void createDefaultObjectTypes();

private:
    void setObjectTypeFields(ObjectType &objectType, const ObjectType &source) const;

    IObjectTypeRepository &repository_;
    IObjectRepository &objectRepository_;

    inline static constexpr std::array<std::string_view, 7> DefaultObjectTypeNames
    {
        "Project",
        "Video",
        "Shorts",
        "Article",
        "Post",
        "Game",
        "Book"
    };
};