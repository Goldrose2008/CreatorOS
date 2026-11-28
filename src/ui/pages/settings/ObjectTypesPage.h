#pragma once

#include <QWidget>

class EntityList;
class LocalizationService;
class ObjectTypeService;

class ObjectTypesPage final : public QWidget
{
    Q_OBJECT

public:
    explicit ObjectTypesPage(ObjectTypeService &objectTypeService, LocalizationService &localization, QWidget *parent = nullptr);

    void reload();

signals:
    void backRequested();

private:
    ObjectTypeService &objectTypeService_;
    LocalizationService &localization_;

    EntityList *objectTypesList_;
};