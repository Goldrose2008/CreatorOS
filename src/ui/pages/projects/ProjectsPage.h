#pragma once

#include <QWidget>
#include <cstdint>

class EntityList;
class EmptyState;
class ErrorState;
class LoadingState;
class LocalizationService;
class PageHeader;
class ProjectService;
class QStackedWidget;

class ProjectsPage final : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectsPage(ProjectService &projectService, LocalizationService &localization, QWidget *parent = nullptr);

    void reload();

signals:
    void projectOpenRequested(std::int64_t projectId);

private:
    ProjectService &projectService_;
    LocalizationService &localization_;

    PageHeader *header_;

    EntityList *projectList_;

    LoadingState *loadingState_;
    EmptyState *emptyState_;
    ErrorState *errorState_;

    QStackedWidget *stateStack_;
};