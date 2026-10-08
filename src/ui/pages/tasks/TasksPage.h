#pragma once

#include <QWidget>

class EntityList;
class EmptyState;
class ErrorState;
class LoadingState;
class LocalizationService;
class PageHeader;
class QStackedWidget;
class TaskService;

class TasksPage final : public QWidget
{
    Q_OBJECT

public:
    explicit TasksPage(TaskService &taskService, LocalizationService &localization, QWidget *parent = nullptr);

    void reload();

private:
    TaskService &taskService_;
    LocalizationService &localization_;

    PageHeader *header_;

    EntityList *taskList_;

    LoadingState *loadingState_;
    EmptyState *emptyState_;
    ErrorState *errorState_;

    QStackedWidget *stateStack_;
};