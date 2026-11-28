#pragma once

#include <QWidget>

class AccountService;
class ContentService;
class LocalizationService;
class NavigationController;
class ObjectTypeService;
class PlatformService;
class ProjectService;
class Sidebar;
class TaskService;
class Workspace;

class AppShell final : public QWidget
{
    Q_OBJECT

public:
    explicit AppShell(
        AccountService &accountService,
        ContentService &contentService,
        ObjectTypeService &objectTypeService,
        PlatformService &platformService,
        ProjectService &projectService, 
        TaskService &taskService,
        QWidget *parent = nullptr
    );

private:
    LocalizationService *localization_;
    Sidebar *sidebar_;
    Workspace *workspace_;
    NavigationController *navigation_;
    ProjectService *projectService_;
    ContentService *contentService_;
    TaskService *taskService_;
};