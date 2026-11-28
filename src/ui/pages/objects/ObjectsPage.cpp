
#include "ObjectsPage.h"

#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonValue>
#include <QLabel>
#include <QStringList>
#include <QVBoxLayout>
#include <QtMath>

#include <exception>

#include "../../components/entity/EntityCard.h"
#include "../../components/entity/EntityList.h"
#include "../../components/layout/PageHeader.h"
#include "../../localization/LocalizationService.h"
#include "../../style/Metrics.h"
#include "../../../application/objects/ObjectService.h"
#include "../../../application/object_types/ObjectTypeService.h"
#include "../../../application/properties/PropertyDefinitionService.h"
#include "../../../application/properties/PropertyValueService.h"
#include "../../../domain/models/Object.h"
#include "../../../domain/models/ObjectType.h"
#include "../../../domain/models/PropertyDefinition.h"
#include "../../../domain/models/PropertyValue.h"

ObjectsPage::ObjectsPage(
    ObjectService &objectService,
    ObjectTypeService &objectTypeService,
    PropertyDefinitionService &propertyDefinitionService,
    PropertyValueService &propertyValueService,
    LocalizationService &localization,
    QWidget *parent
)
    : QWidget(parent),
      objectService_(objectService),
      objectTypeService_(objectTypeService),
      propertyDefinitionService_(propertyDefinitionService),
      propertyValueService_(propertyValueService),
      localization_(localization),
      objectList_(new EntityList(localization_, this))
{
    auto *layout = new QVBoxLayout(this);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(CreatorMetrics::SpacingXLarge);

    auto *header = new PageHeader(this);
    header->setTitle(localization_.text(QStringLiteral("objects")));
    header->setDescription(localization_.text(QStringLiteral("objects.description")));

    objectList_->setEmptyState(QString(), localization_.text(QStringLiteral("objects.empty.description")));
    layout->addWidget(header);
    layout->addWidget(objectList_, 1);

    connect(
        objectList_,
        &EntityList::retryRequested,
        this,
        &ObjectsPage::reload
    );

    reload();
}

void ObjectsPage::reload()
{
    objectList_->showLoading();

    try
    {
        const std::vector<Object> objects = objectService_.getAllActiveObjects();
        const std::vector<ObjectType> objectTypes = objectTypeService_.getAllObjectTypes();
        const std::vector<PropertyDefinition> commonProperties = propertyDefinitionService_.getCommonProperties();

        QHash<qint64, QString> objectTypeNames;

        for (const ObjectType &objectType : objectTypes)
            objectTypeNames.insert(objectType.id, QString::fromUtf8(objectType.name.c_str()));

        QHash<qint64, std::vector<PropertyDefinition>> propertiesByObjectType;

        for (const ObjectType &objectType : objectTypes)
            propertiesByObjectType.insert(objectType.id, propertyDefinitionService_.getPropertiesByObjectTypeID(objectType.id));

        objectList_->setItems(
            objects,
            [this, objectTypeNames, commonProperties, propertiesByObjectType](const Object &object) -> QWidget *
            {
                auto *item = new EntityCard(localization_, objectList_);
                item->setTitle(QString::fromUtf8(object.name.c_str()));

                if (object.objectTypeId.has_value())
                {
                    const auto typeIterator = objectTypeNames.constFind(object.objectTypeId.value());

                    if (typeIterator != objectTypeNames.cend())
                    {
                        auto *typeLabel = new QLabel(localization_.text(QStringLiteral("objects.type")) + QStringLiteral(": ") + typeIterator.value(), item);
                        item->addMetaWidget(typeLabel);
                    }
                }

                if (object.progress.has_value())
                    item->setProgress(qRound(object.progress.value()));

                if (object.deadlineAt.has_value())
                    item->setDeadline(localization_.text(QStringLiteral("objects.deadline")), QString::fromUtf8(object.deadlineAt->c_str()));

                std::vector<PropertyDefinition> applicableProperties;

                for (const PropertyDefinition &definition : commonProperties)
                {
                    if (definition.isEnabled && definition.showOnCard)
                        applicableProperties.push_back(definition);
                }

                if (object.objectTypeId.has_value())
                {
                    const auto propertiesIterator = propertiesByObjectType.constFind(object.objectTypeId.value());

                    if (propertiesIterator != propertiesByObjectType.cend())
                    {
                        for (const PropertyDefinition &definition : propertiesIterator.value())
                        {
                            if (definition.isEnabled && definition.showOnCard)
                                applicableProperties.push_back(definition);
                        }
                    }
                }

                const std::vector<PropertyValue> propertyValues = propertyValueService_.getPropertyValuesForObject(object.id);

                QHash<qint64, QString> valuesByProperty;

                for (const PropertyValue &propertyValue : propertyValues)
                    valuesByProperty.insert(propertyValue.propertyId, QString::fromUtf8(propertyValue.value.c_str()));

                for (const PropertyDefinition &definition : applicableProperties)
                {
                    const auto valueIterator = valuesByProperty.constFind(definition.id);

                    if (valueIterator == valuesByProperty.cend())
                        continue;

                    const QString formattedValue = formatPropertyValue(definition, valueIterator.value().toStdString());

                    if (formattedValue.isEmpty())
                        continue;

                    auto *propertyLabel = new QLabel(QString::fromUtf8(definition.name.c_str()) + QStringLiteral(": ") + formattedValue, item);
                    item->addContentWidget(propertyLabel);
                }

                return item;
            }
        );
    }
    catch (const std::exception &)
    {
        objectList_->showError();
    }
}

QString ObjectsPage::formatPropertyValue(const PropertyDefinition &definition, const std::string &value) const
{
    const QByteArray json = QByteArray::fromStdString(value);

    QJsonArray values;

    if (definition.isMultiple)
    {
        QJsonParseError parseError;
        const QJsonDocument document = QJsonDocument::fromJson(json, &parseError);

        if (parseError.error != QJsonParseError::NoError || !document.isArray())
            return QString::fromUtf8(value.c_str());

        values = document.array();
    }
    else
    {
        QJsonParseError parseError;
        const QByteArray wrappedJson = QByteArray("[") + json + QByteArray("]");
        const QJsonDocument document = QJsonDocument::fromJson(wrappedJson, &parseError);

        if (parseError.error != QJsonParseError::NoError || !document.isArray() || document.array().size() != 1)
            return QString::fromUtf8(value.c_str());

        values.append(document.array().first());
    }

    QStringList formattedValues;

    for (const QJsonValue &item : values)
    {
        switch (definition.valueType)
        {
        case PropertyValueType::Text:
        case PropertyValueType::Date:
        case PropertyValueType::DateTime:
        case PropertyValueType::EntityReference:
            formattedValues.append(item.toString());
            break;
        case PropertyValueType::Number:
            formattedValues.append(QString::number(item.toDouble(), 'g', 15));
            break;
        case PropertyValueType::Boolean:
            formattedValues.append(localization_.text(item.toBool() ? QStringLiteral("common.boolean.true") : QStringLiteral("common.boolean.false")));
            break;
        }
    }

    return formattedValues.join(QStringLiteral(", "));
}