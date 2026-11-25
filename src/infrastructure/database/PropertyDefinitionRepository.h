#pragma once

#include <QSqlDatabase>

#include "../../application/properties/IPropertyDefinitionRepository.h"

class QSqlQuery;
class QString;
class DatabaseManager;

class PropertyDefinitionRepository final : public IPropertyDefinitionRepository
{
public:
    explicit PropertyDefinitionRepository(const DatabaseManager &databaseManager);

    std::vector<PropertyDefinition> findCommon() const override;
    std::vector<PropertyDefinition> findByObjectTypeId(std::int64_t objectTypeId) const override;
    std::optional<PropertyDefinition> findById(std::int64_t id) const override;

    bool existsByName(
        const std::string &name,
        PropertyDefinitionScope scope,
        const std::optional<std::int64_t> &objectTypeId,
        const std::optional<std::int64_t> &excludeId = std::nullopt
    ) const override;

    std::int64_t create(
        const PropertyDefinition &definition,
        const std::optional<PropertyTextSettings> &textSettings,
        const std::optional<PropertyEntityReferenceSettings> &entityReferenceSettings,
        const std::vector<PropertyOption> &options
    ) override;
    bool update(
        const PropertyDefinition &definition,
        const std::optional<PropertyTextSettings> &textSettings,
        const std::optional<PropertyEntityReferenceSettings> &entityReferenceSettings,
        const std::vector<PropertyOption> &options
    ) override;
    bool remove(std::int64_t id) override;

    std::optional<PropertyTextSettings> findTextSettings(std::int64_t propertyId) const override;
    std::optional<PropertyEntityReferenceSettings> findEntityReferenceSettings(std::int64_t propertyId) const override;
    std::vector<PropertyOption> findOptions(std::int64_t propertyId) const override;

private:
    static PropertyDefinition readPropertyDefinition(const QSqlQuery &query);
    static QString selectColumns();
    static void bindDefinitionFields(QSqlQuery &query, const PropertyDefinition &definition);

    static void saveTextSettings(QSqlDatabase &database, std::int64_t propertyId, const std::optional<PropertyTextSettings> &settings);
    static void saveEntityReferenceSettings(QSqlDatabase &database, std::int64_t propertyId, const std::optional<PropertyEntityReferenceSettings> &settings);
    static void saveOptions(QSqlDatabase &database, std::int64_t propertyId, const std::vector<PropertyOption> &options);

    QSqlDatabase database_;
};