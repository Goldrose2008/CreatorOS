#include "PropertyDefinitionService.h"

#include <QDate>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonValue>
#include <QString>
#include <QUuid>

#include <cmath>
#include <stdexcept>
#include <unordered_set>

#include "IPropertyDefinitionRepository.h"
#include "../common/ServiceUtils.h"
#include "../common/StringUtils.h"
#include "../object_types/IObjectTypeRepository.h"

PropertyDefinitionService::PropertyDefinitionService(IPropertyDefinitionRepository &repository, IObjectTypeRepository &objectTypeRepository)
    : repository_(repository), objectTypeRepository_(objectTypeRepository)
{
}

std::vector<PropertyDefinition> PropertyDefinitionService::getCommonProperties() const
{
    return repository_.findCommon();
}

std::vector<PropertyDefinition> PropertyDefinitionService::getPropertiesByObjectTypeID(std::int64_t objectTypeId) const
{
    if (!CreatorServiceUtils::findEntityByID(objectTypeRepository_, objectTypeId).has_value())
        return {};

    return repository_.findByObjectTypeId(objectTypeId);
}

std::optional<PropertyDefinition> PropertyDefinitionService::getPropertyDefinitionByID(std::int64_t id) const
{
    return CreatorServiceUtils::findEntityByID(repository_, id);
}

std::optional<PropertyTextSettings> PropertyDefinitionService::getTextSettings(std::int64_t propertyId) const
{
    if (propertyId <= 0)
        return std::nullopt;

    return repository_.findTextSettings(propertyId);
}

std::optional<PropertyEntityReferenceSettings> PropertyDefinitionService::getEntityReferenceSettings(std::int64_t propertyId) const
{
    if (propertyId <= 0)
        return std::nullopt;

    return repository_.findEntityReferenceSettings(propertyId);
}

std::vector<PropertyOption> PropertyDefinitionService::getOptions(std::int64_t propertyId) const
{
    if (propertyId <= 0)
        return {};

    return repository_.findOptions(propertyId);
}

bool PropertyDefinitionService::isPropertyNameValid(
    const std::string &name,
    PropertyDefinitionScope scope,
    const std::optional<std::int64_t> &objectTypeId,
    const std::optional<std::int64_t> &excludeId
) const
{
    const std::string normalizedName = CreatorStringUtils::trim(name);

    if (normalizedName.empty())
        return false;

    if (excludeId.has_value() && excludeId.value() <= 0)
        return false;

    switch (scope)
    {
    case PropertyDefinitionScope::Common:
        if (objectTypeId.has_value())
            return false;
        break;

    case PropertyDefinitionScope::ObjectType:
        if (!objectTypeId.has_value() || objectTypeId.value() <= 0)
            return false;

        if (!CreatorServiceUtils::findEntityByID(objectTypeRepository_, objectTypeId.value()).has_value())
            return false;
        break;

    default:
        return false;
    }

    return !repository_.existsByName(normalizedName, scope, objectTypeId, excludeId);
}

void PropertyDefinitionService::setPropertyDefinitionFields(PropertyDefinition &definition, const PropertyDefinition &source) const
{
    if (source.scope == PropertyDefinitionScope::Common)
    {
        if (source.objectTypeId.has_value())
            throw std::invalid_argument("Common property cannot belong to an object type.");
    }
    else if (source.scope == PropertyDefinitionScope::ObjectType)
    {
        if (!source.objectTypeId.has_value() || source.objectTypeId.value() <= 0)
            throw std::invalid_argument("Object type is required for this property.");

        if (!CreatorServiceUtils::findEntityByID(objectTypeRepository_, source.objectTypeId.value()).has_value())
            throw std::invalid_argument("Object type was not found.");
    }
    else
    {
        throw std::invalid_argument("Unknown property scope.");
    }

    const std::string normalizedName = CreatorStringUtils::trim(source.name);

    if (normalizedName.empty())
        throw std::invalid_argument("Property name cannot be empty.");

    if (!isPropertyNameValid(
        normalizedName,
        source.scope,
        source.objectTypeId,
        definition.id > 0 ? std::optional<std::int64_t>(definition.id) : std::nullopt
    ))
    {
        throw std::invalid_argument("Property name already exists.");
    }

    definition.scope = source.scope;
    definition.objectTypeId = source.objectTypeId;
    definition.name = normalizedName;
    definition.description = source.description;
    definition.valueType = source.valueType;
    definition.isMultiple = source.isMultiple;
    definition.isRequired = source.isRequired;
    definition.defaultValue = source.defaultValue;
    definition.isEnabled = source.isEnabled;
    definition.showOnCard = source.showOnCard;
}

void PropertyDefinitionService::validateAndNormalizeDefinitionData(
    PropertyDefinition &definition,
    std::optional<PropertyTextSettings> &textSettings,
    std::optional<PropertyEntityReferenceSettings> &entityReferenceSettings,
    std::vector<PropertyOption> &options
) const
{
    // Проверяем настройки, относящиеся к типу значения.
    if (definition.valueType == PropertyValueType::Text)
    {
        if (entityReferenceSettings.has_value())
            throw std::invalid_argument("Text property cannot have entity reference settings.");

        if (textSettings.has_value())
        {
            if (textSettings->minLength.has_value() && textSettings->minLength.value() < 0)
                throw std::invalid_argument("Minimum text length cannot be negative.");

            if (textSettings->maxLength.has_value() && textSettings->maxLength.value() < 0)
                throw std::invalid_argument("Maximum text length cannot be negative.");

            if (textSettings->minLength.has_value() && textSettings->maxLength.has_value() && textSettings->minLength.value() > textSettings->maxLength.value())
                throw std::invalid_argument("Minimum text length cannot exceed maximum text length.");
        }
    }
    else if (definition.valueType == PropertyValueType::EntityReference)
    {
        if (textSettings.has_value())
            throw std::invalid_argument("Entity reference property cannot have text settings.");

        if (!entityReferenceSettings.has_value())
            throw std::invalid_argument("Entity reference target type is required.");

        entityReferenceSettings->targetEntityType = CreatorStringUtils::trim(entityReferenceSettings->targetEntityType);

        if (entityReferenceSettings->targetEntityType.empty())
            throw std::invalid_argument("Entity reference target type is required.");
    }
    else
    {
        if (textSettings.has_value() || entityReferenceSettings.has_value())
            throw std::invalid_argument("The selected property type does not support the supplied settings.");
    }

    // Проверяем одно JSON-значение и возвращаем его каноническое представление.
    const auto normalizeValue = [&definition](const QJsonValue &value) -> std::pair<QString, QString>
    {
        QString normalizedValue;
        QString canonicalValue;

        switch (definition.valueType)
        {
        case PropertyValueType::Text:
        {
            if (!value.isString())
                throw std::invalid_argument("Property value must be a JSON string.");

            canonicalValue = value.toString();
            normalizedValue = canonicalValue.toCaseFolded();
            break;
        }

        case PropertyValueType::Number:
        {
            if (!value.isDouble() || !std::isfinite(value.toDouble()))
                throw std::invalid_argument("Property value must be a valid JSON number.");

            canonicalValue = QString::number(value.toDouble(), 'g', 17);
            normalizedValue = canonicalValue;
            break;
        }

        case PropertyValueType::Boolean:
        {
            if (!value.isBool())
                throw std::invalid_argument("Property value must be a JSON boolean.");

            canonicalValue = value.toBool() ? QStringLiteral("true") : QStringLiteral("false");
            normalizedValue = canonicalValue;
            break;
        }

        case PropertyValueType::Date:
        {
            if (!value.isString())
                throw std::invalid_argument("Property value must be a date string.");

            const QString dateValue = value.toString();
            const QDate date = QDate::fromString(dateValue, Qt::ISODate);

            if (!date.isValid() || date.toString(Qt::ISODate) != dateValue)
                throw std::invalid_argument("Date must use YYYY-MM-DD format.");

            canonicalValue = date.toString(Qt::ISODate);
            normalizedValue = canonicalValue;
            break;
        }

        case PropertyValueType::DateTime:
        {
            if (!value.isString())
                throw std::invalid_argument("Property value must be a date-time string.");

            const QDateTime dateTime = QDateTime::fromString(value.toString(), Qt::ISODateWithMs);

            if (!dateTime.isValid())
                throw std::invalid_argument("Property value must be a valid ISO 8601 date-time.");

            canonicalValue = dateTime.toUTC().toString(Qt::ISODateWithMs);
            normalizedValue = canonicalValue;
            break;
        }

        case PropertyValueType::EntityReference:
        {
            if (!value.isString())
                throw std::invalid_argument("Entity reference must be a UUID string.");

            const QUuid uuid(QString::fromUtf8(value.toString().toUtf8()));

            if (uuid.isNull())
                throw std::invalid_argument("Entity reference must contain a valid UUID.");

            canonicalValue = uuid.toString(QUuid::WithoutBraces).toLower();
            normalizedValue = canonicalValue;
            break;
        }

        default:
            throw std::invalid_argument("Unknown property value type.");
        }

        return {normalizedValue, canonicalValue};
    };

    // Проверяем default_value.
    if (definition.defaultValue.has_value())
    {
        QJsonParseError parseError;
        QJsonArray defaultValues;

        if (definition.isMultiple)
        {
            const QJsonDocument document = QJsonDocument::fromJson(QByteArray::fromStdString(definition.defaultValue.value()), &parseError);

            if (parseError.error != QJsonParseError::NoError || !document.isArray())
                throw std::invalid_argument("Multiple property default value must be a JSON array.");

            defaultValues = document.array();
        }
        else
        {
            const QByteArray wrappedJson = QByteArray("[") + QByteArray::fromStdString(definition.defaultValue.value()) + QByteArray("]");
            const QJsonDocument document = QJsonDocument::fromJson(wrappedJson, &parseError);

            if (parseError.error != QJsonParseError::NoError || !document.isArray() || document.array().size() != 1)
                throw std::invalid_argument("Single property default value must be a valid JSON scalar.");

            defaultValues.append(document.array().first());
        }

        std::unordered_set<std::string> uniqueValues;
        QJsonArray normalizedDefaultValues;

        for (const QJsonValue &value : defaultValues)
        {
            const auto [normalized, canonical] = normalizeValue(value);

            if (!uniqueValues.insert(normalized.toStdString()).second)
                throw std::invalid_argument("Property default value cannot contain duplicates.");

            if (definition.valueType == PropertyValueType::Text && textSettings.has_value())
            {
                const std::int64_t length = value.toString().size();

                if (textSettings->minLength.has_value() && length < textSettings->minLength.value())
                    throw std::invalid_argument("Default text value is shorter than the minimum length.");

                if (textSettings->maxLength.has_value() && length > textSettings->maxLength.value())
                    throw std::invalid_argument("Default text value exceeds the maximum length.");
            }

            QJsonParseError valueError;
            const QByteArray wrappedCanonical = QByteArray("[") + canonical.toUtf8() + QByteArray("]");
            const QJsonDocument canonicalDocument = QJsonDocument::fromJson(wrappedCanonical, &valueError);

            if (valueError.error != QJsonParseError::NoError || !canonicalDocument.isArray())
                throw std::invalid_argument("Could not normalize property default value.");

            normalizedDefaultValues.append(canonicalDocument.array().first());
        }

        if (definition.isMultiple)
        {
            definition.defaultValue = QJsonDocument(normalizedDefaultValues).toJson(QJsonDocument::Compact).toStdString();
        }
        else
        {
            const QByteArray json =QJsonDocument(normalizedDefaultValues).toJson(QJsonDocument::Compact);
            definition.defaultValue = json.mid(1, json.size() - 2).toStdString();
        }
    }

    // Проверяем и нормализуем варианты.
    std::unordered_set<std::string> uniqueOptions;

    for (PropertyOption &option : options)
    {
        QJsonParseError parseError;

        const QByteArray wrappedJson = QByteArray("[") + QByteArray::fromStdString(option.value) + QByteArray("]");
        const QJsonDocument document = QJsonDocument::fromJson(wrappedJson, &parseError);

        if (parseError.error != QJsonParseError::NoError || !document.isArray() || document.array().size() != 1)
            throw std::invalid_argument("Property option must contain one valid JSON scalar.");

        const QJsonValue value = document.array().first();
        const auto [normalized, canonical] = normalizeValue(value);

        if (!uniqueOptions.insert(normalized.toStdString()).second)
            throw std::invalid_argument("Property options cannot contain duplicates.");

        if (definition.valueType == PropertyValueType::Text && textSettings.has_value())
        {
            const std::int64_t length = value.toString().size();

            if (textSettings->minLength.has_value() && length < textSettings->minLength.value())
                throw std::invalid_argument("Text option is shorter than the minimum length.");

            if (textSettings->maxLength.has_value() && length > textSettings->maxLength.value())
                throw std::invalid_argument("Text option exceeds the maximum length.");
        }

        QJsonParseError canonicalError;
        const QByteArray canonicalJson = QByteArray("[") + canonical.toUtf8() + QByteArray("]");
        const QJsonDocument canonicalDocument = QJsonDocument::fromJson(canonicalJson, &canonicalError);

        if (canonicalError.error != QJsonParseError::NoError || !canonicalDocument.isArray())
            throw std::invalid_argument("Could not normalize property option.");

        const QByteArray serialized = QJsonDocument(canonicalDocument.array()).toJson(QJsonDocument::Compact);
        option.value = serialized.mid(1, serialized.size() - 2).toStdString();
        option.normalizedValue = normalized.toStdString();
    }
}






