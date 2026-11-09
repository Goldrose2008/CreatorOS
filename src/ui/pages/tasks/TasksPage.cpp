#include "TasksPage.h"

#include <QVBoxLayout>
#include <QDialog>
#include <QPushButton>
#include <exception>

#include "TaskCard.h"
#include "TaskEditorDialog.h"
#include "../../components/forms/EditorDialog.h"
#include "../../components/entity/EntityList.h"
#include "../../components/layout/PageHeader.h"
#include "../../localization/LocalizationService.h"
#include "../../style/Metrics.h"
#include "../../../application/tasks/TaskService.h"
#include "../../../application/projects/ProjectService.h"
#include "../../../application/content/ContentService.h"
#include "../../../domain/models/Task.h"

TasksPage::TasksPage(TaskService &taskService, ProjectService &projectService, ContentService &contentService, LocalizationService &localization, QWidget *parent)
    : QWidget(parent),
      taskService_(taskService),
      projectService_(projectService),
      contentService_(contentService),
      localization_(localization),
      header_(new PageHeader(this)),
      taskList_(new EntityList(localization_, this))
{
    auto *layout = new QVBoxLayout(this);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(CreatorMetrics::SpacingXLarge);
    
    header_->setTitle(localization_.text(QStringLiteral("tasks")));
    header_->setDescription(localization_.text(QStringLiteral("tasks.description")));
    
    auto *createButton = new QPushButton(localization_.text(QStringLiteral("tasks.create")), this);
    header_->addAction(createButton);
    
    connect(
        createButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            TaskEditorDialog dialog(taskService_, projectService_, contentService_, localization_, this);
            
            if (dialog.exec() == QDialog::Accepted)
                reload();
        });

    layout->addWidget(header_);
    layout->addWidget(taskList_, 1);

    connect(
        taskList_,
        &EntityList::retryRequested,
        this,
        &TasksPage::reload
    );

    reload();
}

void TasksPage::reload()
{
    taskList_->showLoading();

    try
    {
        const std::vector<Task> tasks = taskService_.getAllTasks();

        taskList_->setItems(
            tasks,
            [this](const Task &task) -> QWidget *
            {
                auto *card = new TaskCard(task, localization_, taskList_);

                connect(
                    card,
                    &TaskCard::openRequested,
                    this,
                    &TasksPage::taskOpenRequested
                );

                return card;
            }
        );
    }
    catch (const std::exception &)
    {
        taskList_->showError();
    }
}