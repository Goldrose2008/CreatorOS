#pragma once

#include <QWidget>

class LocalizationService;
class Sidebar;
class Workspace;
class NavigationController;
class ProjectService;

class AppShell final : public QWidget
{
    Q_OBJECT

public:
    explicit AppShell(ProjectService &projectService, QWidget *parent = nullptr);

private:
    LocalizationService *localization_;
    Sidebar *sidebar_;
    Workspace *workspace_;
    NavigationController *navigation_;
    ProjectService *projectService_;
};