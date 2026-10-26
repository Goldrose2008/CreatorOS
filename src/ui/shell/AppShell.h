#pragma once

#include <QWidget>

class ContentService;
class LocalizationService;
class NavigationController;
class PlatformService;
class ProjectService;
class Sidebar;
class TaskService;
class Workspace;

class AppShell final : public QWidget
{
    Q_OBJECT

public:
    explicit AppShell(ProjectService &projectService, ContentService &contentService, TaskService &taskService, PlatformService &platformService, QWidget *parent = nullptr);

private:
    LocalizationService *localization_;
    Sidebar *sidebar_;
    Workspace *workspace_;
    NavigationController *navigation_;
    ProjectService *projectService_;
    ContentService *contentService_;
    TaskService *taskService_;
};