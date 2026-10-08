#pragma once

#include <QWidget>

class LocalizationService;
class Sidebar;
class Workspace;
class NavigationController;
class ContentService;
class ProjectService;
class TaskService;

class AppShell final : public QWidget
{
    Q_OBJECT

public:
    explicit AppShell(ProjectService &projectService, ContentService &contentService, TaskService &taskService, QWidget *parent = nullptr);

private:
    LocalizationService *localization_;
    Sidebar *sidebar_;
    Workspace *workspace_;
    NavigationController *navigation_;
    ProjectService *projectService_;
    ContentService *contentService_;
    TaskService *taskService_;
};