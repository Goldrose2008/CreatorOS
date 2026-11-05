#pragma once

#include <QWidget>
#include <cstdint>

class EntityList;
class LocalizationService;
class ContentService;
class ProjectService;
class TaskService;
class PageHeader;

class TasksPage final : public QWidget
{
    Q_OBJECT

public:
    explicit TasksPage(TaskService &taskService, ProjectService &projectService, ContentService &contentService, LocalizationService &localization, QWidget *parent = nullptr);

    void reload();

signals:
    void taskOpenRequested(std::int64_t taskId);

private:
    TaskService &taskService_;
    ProjectService &projectService_;
    ContentService &contentService_;
    LocalizationService &localization_;

    PageHeader *header_;

    EntityList *taskList_;
};