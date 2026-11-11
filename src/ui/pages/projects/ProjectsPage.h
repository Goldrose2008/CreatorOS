#pragma once

#include <QWidget>
#include <cstdint>

class EntityList;
class LocalizationService;
class PageHeader;
class ContentService;
class ProjectService;

class ProjectsPage final : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectsPage(ProjectService &projectService, ContentService &contentService, LocalizationService &localization, QWidget *parent = nullptr);

    void reload();

signals:
    void projectOpenRequested(std::int64_t projectId);

private:
    ProjectService &projectService_;
    ContentService &contentService_;
    LocalizationService &localization_;

    PageHeader *header_;

    EntityList *projectList_;
};