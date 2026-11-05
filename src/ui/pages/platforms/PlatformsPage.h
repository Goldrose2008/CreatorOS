
#pragma once

#include <QWidget>

class EntityList;
class LocalizationService;
class PlatformService;

class PlatformsPage final : public QWidget
{
    Q_OBJECT

public:
    explicit PlatformsPage(PlatformService &platformService, LocalizationService &localization, QWidget *parent = nullptr);

    void reload();

private:
    PlatformService &platformService_;
    LocalizationService &localization_;

    EntityList *platformList_;
};
