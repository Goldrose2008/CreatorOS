#include "PropertyValueService.h"

#include <QByteArray>
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
#include <utility>

#include "IPropertyValueRepository.h"
#include "IPropertyDefinitionRepository.h"
#include "../objects/IObjectRepository.h"

PropertyValueService::PropertyValueService(IPropertyValueRepository &valueRepository, IPropertyDefinitionRepository &definitionRepository, IObjectRepository &objectRepository)
    : valueRepository_(valueRepository), definitionRepository_(definitionRepository), objectRepository_(objectRepository)
{
}

std::vector<PropertyValue> PropertyValueService::getPropertyValuesForObject(std::int64_t objectId) const
{
    if (objectId <= 0)
        return {};

    if (!objectRepository_.findById(objectId).has_value())
        return {};

    return valueRepository_.findByObjectId(objectId);
}

std::optional<PropertyValue> PropertyValueService::getPropertyValue(std::int64_t objectId, std::int64_t propertyId) const
{
    if (objectId <= 0 || propertyId <= 0)
        return std::nullopt;

    if (!objectRepository_.findById(objectId).has_value())
        return std::nullopt;

    return valueRepository_.findByObjectAndProperty(objectId, propertyId);
}

void PropertyValueService::validatePropertyApplicability(const PropertyDefinition &definition, const Object &object) const
{
    if (!definition.isEnabled)
        throw std::invalid_argument("Property is disabled.");

    if (definition.scope == PropertyDefinitionScope::Common)
    {
        if (definition.objectTypeId.has_value())
            throw std::invalid_argument("Common property cannot belong to an object type.");

        return;
    }

    if (definition.scope == PropertyDefinitionScope::ObjectType)
    {
        if (!definition.objectTypeId.has_value())
            throw std::invalid_argument("Object type is required for this property.");

        if (!object.objectTypeId.has_value() || object.objectTypeId.value() != definition.objectTypeId.value())
            throw std::invalid_argument("Property does not apply to this object.");

        return;
    }

    throw std::invalid_argument("Unknown property scope.");
}

std::optional<std::string> PropertyValueService::validateAndNormalizeValue(
    const PropertyDefinition &definition,
    const std::optional<PropertyTextSettings> &textSettings,
    const std::optional<PropertyEntityReferenceSettings> &entityReferenceSettings,
    const std::string &sourceValue
) const
{
    switch (definition.valueType)
    {
    case PropertyValueType::Text:
    case PropertyValueType::Number:
    case PropertyValueType::Boolean:
    case PropertyValueType::Date:
    case PropertyValueType::DateTime:
    case PropertyValueType::EntityReference:
        break;

    default:
        throw std::invalid_argument("Unknown property value type.");
    }

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

        if (!entityReferenceSettings.has_value() || entityReferenceSettings->targetEntityType.empty())
            throw std::invalid_argument("Entity reference target type is required.");
    }
    else
    {
        if (textSettings.has_value() || entityReferenceSettings.has_value())
            throw std::invalid_argument("The selected property type does not support the supplied settings.");
    }

    const auto normalizeValue = [&definition, &textSettings](const QJsonValue &value) -> std::pair<QString, QJsonValue>
    {
        QString normalizedValue;
        QJsonValue canonicalValue;

        switch (definition.valueType)
        {
        case PropertyValueType::Text:
        {
            if (!value.isString())
                throw std::invalid_argument("Property value must be a JSON string.");

            const QString text = value.toString();

            if (text.trimmed().isEmpty())
                return {QString(), QJsonValue()};

            if (textSettings.has_value())
            {
                const std::int64_t length = text.size();

                if (textSettings->minLength.has_value() && length < textSettings->minLength.value())
                    throw std::invalid_argument("Text value is shorter than the minimum length.");

                if (textSettings->maxLength.has_value() && length > textSettings->maxLength.value())
                    throw std::invalid_argument("Text value exceeds the maximum length.");
            }

            canonicalValue = QJsonValue(text);
            normalizedValue = text.toCaseFolded();
            break;
        }

        case PropertyValueType::Number:
        {
            if (!value.isDouble() || !std::isfinite(value.toDouble()))
                throw std::invalid_argument("Property value must be a valid JSON number.");

            canonicalValue = QJsonValue(value.toDouble());
            normalizedValue = QString::number(value.toDouble(), 'g', 17);
            break;
        }

        case PropertyValueType::Boolean:
        {
            if (!value.isBool())
                throw std::invalid_argument("Property value must be a JSON boolean.");

            canonicalValue = QJsonValue(value.toBool());
            normalizedValue = value.toBool() ? QStringLiteral("true") : QStringLiteral("false");
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

            canonicalValue = QJsonValue(date.toString(Qt::ISODate));
            normalizedValue = date.toString(Qt::ISODate);
            break;
        }

        case PropertyValueType::DateTime:
        {
            if (!value.isString())
                throw std::invalid_argument("Property value must be a date-time string.");

            const QDateTime dateTime =
                QDateTime::fromString(value.toString(), Qt::ISODateWithMs);

            if (!dateTime.isValid())
                throw std::invalid_argument("Property value must be a valid ISO 8601 date-time.");

            const QString utcValue = dateTime.toUTC().toString(Qt::ISODateWithMs);

            canonicalValue = QJsonValue(utcValue);
            normalizedValue = utcValue;
            break;
        }

        case PropertyValueType::EntityReference:
        {
            if (!value.isString())
                throw std::invalid_argument("Entity reference must be a UUID string.");

            const QUuid uuid(value.toString());

            if (uuid.isNull())
                throw std::invalid_argument("Entity reference must contain a valid UUID.");

            const QString uuidValue = uuid.toString(QUuid::WithoutBraces).toLower();

            canonicalValue = QJsonValue(uuidValue);
            normalizedValue = uuidValue;
            break;
        }

        default:
            throw std::invalid_argument("Unknown property value type.");
        }

        return {normalizedValue, canonicalValue};
    };

    QJsonArray values;

    if (definition.isMultiple)
    {
        QJsonParseError parseError;

        const QJsonDocument document = QJsonDocument::fromJson(QByteArray::fromStdString(sourceValue), &parseError);

        if (parseError.error != QJsonParseError::NoError || !document.isArray())
            throw std::invalid_argument("Multiple property value must be a JSON array.");

        values = document.array();
    }
    else
    {
        QJsonParseError parseError;

        const QByteArray wrappedJson = QByteArray("[") + QByteArray::fromStdString(sourceValue) + QByteArray("]");
        const QJsonDocument document = QJsonDocument::fromJson(wrappedJson, &parseError);

        if (parseError.error != QJsonParseError::NoError || !document.isArray() || document.array().size() != 1)
            throw std::invalid_argument("Single property value must be a valid JSON scalar.");

        values.append(document.array().first());
    }

    std::unordered_set<std::string> uniqueValues;
    QJsonArray normalizedValues;

    for (const QJsonValue &value : values)
    {
        const auto [normalized, canonical] = normalizeValue(value);

        // Пустой текст трактуется как отсутствие значения.
        if (normalized.isNull())
            continue;

        if (!uniqueValues.insert(normalized.toStdString()).second)
            throw std::invalid_argument("Property value cannot contain duplicates.");

        normalizedValues.append(canonical);
    }

    if (!definition.isMultiple && normalizedValues.isEmpty())
        return std::nullopt;

    if (definition.isMultiple)
        return QJsonDocument(normalizedValues).toJson(QJsonDocument::Compact).toStdString();

    QJsonArray scalarArray;
    scalarArray.append(normalizedValues.first());

    const QByteArray serialized =QJsonDocument(scalarArray).toJson(QJsonDocument::Compact);
    return serialized.mid(1, serialized.size() - 2).toStdString();
}

bool PropertyValueService::savePropertyValue(std::int64_t objectId, std::int64_t propertyId, const std::string &sourceValue)
{
    const auto object = objectRepository_.findById(objectId);

    if (!object.has_value() || object->deletedAt.has_value())
        return false;

    const auto definition = definitionRepository_.findById(propertyId);

    if (!definition.has_value())
        return false;

    validatePropertyApplicability(definition.value(), object.value());

    const auto textSettings = definitionRepository_.findTextSettings(propertyId);
    const auto entityReferenceSettings = definitionRepository_.findEntityReferenceSettings(propertyId);
    const auto normalizedValue = validateAndNormalizeValue(definition.value(), textSettings, entityReferenceSettings, sourceValue);

    // Пустое значение означает отсутствие собственной записи.
    if (!normalizedValue.has_value())
        return removePropertyValue(objectId, propertyId);

    const auto existingValue = valueRepository_.findByObjectAndProperty(objectId, propertyId);

    // Повторное сохранение того же значения не изменяет Object.
    if (existingValue.has_value() && existingValue->value == normalizedValue.value())
        return false;

    PropertyValue value;
    value.objectId = objectId;
    value.propertyId = propertyId;
    value.value = normalizedValue.value();

    const std::string updatedAt =QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs).toStdString();

    return valueRepository_.save(value, updatedAt);
}

bool PropertyValueService::removePropertyValue(std::int64_t objectId, std::int64_t propertyId)
{
    const auto object = objectRepository_.findById(objectId);

    if (!object.has_value() || object->deletedAt.has_value())
        return false;

    if (objectId <= 0 || propertyId <= 0)
        return false;

    const std::string updatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs).toStdString();

    return valueRepository_.remove(objectId, propertyId, updatedAt);
}