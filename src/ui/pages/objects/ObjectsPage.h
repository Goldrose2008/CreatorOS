
#pragma once

#include <QWidget>

class EntityList;
class LocalizationService;
class ObjectService;
class ObjectTypeService;

class ObjectsPage final : public QWidget
{
    Q_OBJECT

public:
    explicit ObjectsPage(
        ObjectService &objectService,
        ObjectTypeService &objectTypeService,
        LocalizationService &localization,
        QWidget *parent = nullptr
    );

    void reload();

private:
    ObjectService &objectService_;
    ObjectTypeService &objectTypeService_;
    LocalizationService &localization_;

    EntityList *objectList_;
};
