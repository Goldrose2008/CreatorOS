#include "TasksPage.h"

#include <QVBoxLayout>
#include <QStackedWidget>
#include <exception>

#include "TaskCard.h"
#include "../../components/lists/EntityList.h"
#include "../../components/layout/PageHeader.h"
#include "../../components/states/EmptyState.h"
#include "../../components/states/ErrorState.h"
#include "../../components/states/LoadingState.h"
#include "../../localization/LocalizationService.h"
#include "../../style/Metrics.h"
#include "../../../application/tasks/TaskService.h"
#include "../../../domain/models/Task.h"

TasksPage::TasksPage(TaskService &taskService, LocalizationService &localization, QWidget *parent)
    : QWidget(parent),
      taskService_(taskService),
      localization_(localization),
      header_(new PageHeader(this)),
      taskList_(new EntityList(this)),
      loadingState_(new LoadingState(this)),
      emptyState_(new EmptyState(this)),
      errorState_(new ErrorState(this)),
      stateStack_(new QStackedWidget(this))
{
    auto *layout = new QVBoxLayout(this);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(CreatorMetrics::SpacingXLarge);
    
    header_->setTitle(localization_.text(QStringLiteral("tasks")));
    header_->setDescription(localization_.text(QStringLiteral("tasks.description")));
    
    loadingState_->setTitle(localization_.text(QStringLiteral("tasks.loading")));
    
    emptyState_->setTitle(localization_.text(QStringLiteral("tasks.empty.title")));
    emptyState_->setDescription(localization_.text(QStringLiteral("tasks.empty.description")));
    
    errorState_->setTitle(localization_.text(QStringLiteral("tasks.error.title")));
    errorState_->setDescription(localization_.text(QStringLiteral("tasks.error.description")));
    errorState_->setRetryText(localization_.text(QStringLiteral("common.action.retry")));

    stateStack_->addWidget(taskList_);
    stateStack_->addWidget(loadingState_);
    stateStack_->addWidget(emptyState_);
    stateStack_->addWidget(errorState_);

    layout->addWidget(header_);
    layout->addWidget(stateStack_, 1);

    connect(
        errorState_,
        &ErrorState::retryRequested,
        this,
        &TasksPage::reload
    );

    reload();
}

void TasksPage::reload()
{
    stateStack_->setCurrentWidget(loadingState_);
    taskList_->clearItems();

    try
    {
        const std::vector<Task> tasks = taskService_.getTasks();

        if (tasks.empty())
        {
            stateStack_->setCurrentWidget(emptyState_);
            return;
        }

        for (const Task &task : tasks)
        {
            auto *card = new TaskCard(
                task,
                localization_,
                taskList_
            );

            taskList_->addItem(card);
        }

        stateStack_->setCurrentWidget(taskList_);
    }
    catch (const std::exception &)
    {
        stateStack_->setCurrentWidget(errorState_);
    }
}