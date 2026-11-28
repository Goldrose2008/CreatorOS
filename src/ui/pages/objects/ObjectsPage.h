
#pragma once

#include <QWidget>
#include <QString>

#include <string>

class EntityList;
class LocalizationService;
class ObjectService;
class ObjectTypeService;
class PropertyDefinitionService;
class PropertyValueService;
struct PropertyDefinition;

class ObjectsPage final : public QWidget
{
    Q_OBJECT

public:
    explicit ObjectsPage(
        ObjectService &objectService,
        ObjectTypeService &objectTypeService,
        PropertyDefinitionService &propertyDefinitionService,
        PropertyValueService &propertyValueService,
        LocalizationService &localization,
        QWidget *parent = nullptr
    );

    void reload();

private:
    ObjectService &objectService_;
    ObjectTypeService &objectTypeService_;
    PropertyDefinitionService &propertyDefinitionService_;
    PropertyValueService &propertyValueService_;
    LocalizationService &localization_;

    EntityList *objectList_;

    QString formatPropertyValue(const PropertyDefinition &definition, const std::string &value) const;
};
