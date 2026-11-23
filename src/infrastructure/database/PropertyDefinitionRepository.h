#pragma once

#include <QSqlDatabase>

#include "../../application/properties/IPropertyDefinitionRepository.h"

class DatabaseManager;

class PropertyDefinitionRepository final : public IPropertyDefinitionRepository
{
public:
    explicit PropertyDefinitionRepository(const DatabaseManager &databaseManager);

    std::vector<PropertyDefinition> findCommon() const override;
    std::vector<PropertyDefinition> findByObjectTypeId(std::int64_t objectTypeId) const override;
    std::optional<PropertyDefinition> findById(std::int64_t id) const override;

    std::int64_t create(const PropertyDefinition &definition) override;
    bool update(const PropertyDefinition &definition) override;
    bool remove(std::int64_t id) override;

    std::optional<PropertyTextSettings> findTextSettings(std::int64_t propertyId) const override;
    bool saveTextSettings(const PropertyTextSettings &settings) override;
    bool removeTextSettings(std::int64_t propertyId) override;

    std::optional<PropertyEntityReferenceSettings> findEntityReferenceSettings(std::int64_t propertyId) const override;
    bool saveEntityReferenceSettings(const PropertyEntityReferenceSettings &settings) override;
    bool removeEntityReferenceSettings(std::int64_t propertyId) override;

    std::vector<PropertyOption> findOptions(std::int64_t propertyId) const override;
    std::int64_t createOption(const PropertyOption &option) override;
    bool updateOption(const PropertyOption &option) override;
    bool removeOption(std::int64_t id) override;

private:
    QSqlDatabase database_;
};