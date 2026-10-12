#pragma once

#include <cstdint>

#include "../../components/entity/EntityDetails.h"
#include "../../../domain/models/Task.h"

class QLabel;
class QPushButton;
class Section;
class EntityHeader;

class ContentService;
class LocalizationService;
class ProjectService;
class TaskService;

class TaskDetailsPage final : public EntityDetails
{
    Q_OBJECT

public:
    explicit TaskDetailsPage(
        TaskService &taskService,
        ProjectService &projectService,
        ContentService &contentService,
        LocalizationService &localization,
        QWidget *parent = nullptr
    );

public slots:
    void showTask(std::int64_t taskId);

signals:
    void backRequested();

private:
    TaskService &taskService_;
    ProjectService &projectService_;
    ContentService &contentService_;
    LocalizationService &localization_;

    Task task_;

    EntityHeader *header_;
    QPushButton *editButton_;
    QPushButton *deleteButton_;

    QLabel *parentLabel_;
    QLabel *statusLabel_;
    QLabel *deadlineLabel_;
    QLabel *createdLabel_;
    QLabel *updatedLabel_;
    QLabel *descriptionLabel_;

    Section *summarySection_;
    Section *descriptionSection_;
};