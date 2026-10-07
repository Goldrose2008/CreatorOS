#pragma once

#include <QWidget>

class LocalizationService;
class Sidebar;
class Workspace;
class NavigationController;
class ProjectService;
class ContentService;

class AppShell final : public QWidget
{
    Q_OBJECT

public:
    explicit AppShell(ProjectService &projectService, ContentService &contentService, QWidget *parent = nullptr);

private:
    LocalizationService *localization_;
    Sidebar *sidebar_;
    Workspace *workspace_;
    NavigationController *navigation_;
    ProjectService *projectService_;
    ContentService *contentService_;
};