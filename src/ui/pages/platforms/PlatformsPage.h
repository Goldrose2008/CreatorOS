
#pragma once

#include <QWidget>

class EntityList;
class EmptyState;
class ErrorState;
class LoadingState;
class LocalizationService;
class PlatformService;
class QStackedWidget;

class PlatformsPage final : public QWidget
{
    Q_OBJECT

public:
    explicit PlatformsPage(
        PlatformService &platformService,
        LocalizationService &localization,
        QWidget *parent = nullptr
    );

    void reload();

private:
    PlatformService &platformService_;
    LocalizationService &localization_;

    EntityList *platformList_;
    LoadingState *loadingState_;
    EmptyState *emptyState_;
    ErrorState *errorState_;
    QStackedWidget *stateStack_;
};
