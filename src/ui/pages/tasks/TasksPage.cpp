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
      taskList_(new EntityList(this))
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

    taskList_->setLoadingTitle(localization_.text(QStringLiteral("common.state.loading")));
    taskList_->setEmptyState(localization_.text(QStringLiteral("common.state.empty.title")), localization_.text(QStringLiteral("common.state.empty.description")));
    taskList_->setErrorState(
        localization_.text(QStringLiteral("common.state.error.title")),
        localization_.text(QStringLiteral("common.state.error.load")),
        localization_.text(QStringLiteral("common.action.retry"))
    );

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
    taskList_->clearItems();

    try
    {
        const std::vector<Task> tasks = taskService_.getTasks();

        if (tasks.empty())
        {
            taskList_->showEmpty();
            return;
        }

        for (const Task &task : tasks)
        {
            auto *card = new TaskCard(task, localization_, taskList_);
           
            connect(
                card,
                &TaskCard::openRequested,
                this,
                &TasksPage::taskOpenRequested
            );
            
            taskList_->addItem(card);
        }

        taskList_->showList();
    }
    catch (const std::exception &)
    {
        taskList_->showError();
    }
}