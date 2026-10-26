#include "TaskDetailsPage.h"

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include <exception>

#include "TaskEditorDialog.h"
#include "../../components/entity/EntityHeader.h"
#include "../../components/forms/ConfirmModal.h"
#include "../../components/layout/Section.h"
#include "../../localization/LocalizationService.h"
#include "../../../application/content/ContentService.h"
#include "../../../application/projects/ProjectService.h"
#include "../../../application/tasks/TaskService.h"
#include "../../../domain/models/Content.h"
#include "../../../domain/models/Project.h"

namespace
{
    QLabel *createLabel(QWidget *parent)
    {
        auto *label = new QLabel(parent);
        label->setWordWrap(true);
        return label;
    }

    QString taskStatusText(TaskStatus status, const LocalizationService &localization)
    {
        switch (status)
        {
        case TaskStatus::Todo:
            return localization.text(QStringLiteral("task.status.todo"));
        case TaskStatus::InProgress:
            return localization.text(QStringLiteral("common.status.in_progress"));
        case TaskStatus::Done:
            return localization.text(QStringLiteral("task.status.done"));
        case TaskStatus::Cancelled:
            return localization.text(QStringLiteral("task.status.cancelled"));
        }
        return QString();
    }
}

TaskDetailsPage::TaskDetailsPage(
    TaskService &taskService,
    ProjectService &projectService,
    ContentService &contentService,
    LocalizationService &localization,
    QWidget *parent
)
    : EntityDetails(parent),
      taskService_(taskService),
      projectService_(projectService),
      contentService_(contentService),
      localization_(localization),
      header_(new EntityHeader(this)),
      editButton_(new QPushButton(localization.text(QStringLiteral("common.action.edit")), this)),
      deleteButton_(new QPushButton(localization.text(QStringLiteral("common.action.delete")), this)),
      parentLabel_(createLabel(this)),
      statusLabel_(createLabel(this)),
      deadlineLabel_(createLabel(this)),
      createdLabel_(createLabel(this)),
      updatedLabel_(createLabel(this)),
      descriptionLabel_(createLabel(this)),
      summarySection_(new Section(this)),
      descriptionSection_(new Section(this))
{
    auto *backButton = new QPushButton(localization_.text(QStringLiteral("common.action.back")), this);

    connect(
        backButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            if (task_.id > 0)
                emit backRequested();
        });

    setNavigationWidget(backButton);
    setHeaderWidget(header_);

    header_->addAction(editButton_);
    header_->addAction(deleteButton_);
    editButton_->setVisible(false);
    deleteButton_->setVisible(false);

    connect(
        editButton_,
        &QPushButton::clicked,
        this,
        [this]()
        {
            if (task_.id <= 0)
                return;

            const std::int64_t taskId = task_.id;
            TaskEditorDialog dialog(taskService_, projectService_, contentService_, localization_, this, task_);
            
            if (dialog.exec() == QDialog::Accepted)
                showTask(taskId);
        });

    connect(
        deleteButton_,
        &QPushButton::clicked,
        this,
        [this]()
        {
            if (task_.id <= 0)
                return;

            const std::int64_t taskId = task_.id;

            ConfirmModal::confirm(
                this,
                localization_.text(QStringLiteral("common.dialog.delete.title")),
                localization_.text(QStringLiteral("common.dialog.delete.description")),
                localization_.text(QStringLiteral("common.action.delete")),
                localization_.text(QStringLiteral("common.action.cancel")),
                [this, taskId]()
                {
                    try
                    {
                        if (!taskService_.deleteTask(taskId))
                        {
                            QMessageBox::critical(this, localization_.text(QStringLiteral("common.state.error.title")), localization_.text(QStringLiteral("task.delete.error.description")));
                            return;
                        }

                        task_ = {};
                        emit backRequested();
                    }
                    catch (const std::exception &)
                    {
                        QMessageBox::critical(this, localization_.text(QStringLiteral("common.state.error.title")), localization_.text(QStringLiteral("task.delete.error.description")));
                    }
                });
        });

    auto *summaryGrid = new QGridLayout();
    summaryGrid->setContentsMargins(0, 0, 0, 0);
    summarySection_->setTitle(localization_.text(QStringLiteral("common.label.summary")));
    summaryGrid->addWidget(new QLabel(localization_.text(QStringLiteral("task.create.parent")), summarySection_), 0, 0);
    summaryGrid->addWidget(parentLabel_, 0, 1);
    summaryGrid->addWidget(new QLabel(localization_.text(QStringLiteral("common.label.status")), summarySection_), 1, 0);
    summaryGrid->addWidget(statusLabel_, 1, 1);
    summaryGrid->addWidget(new QLabel(localization_.text(QStringLiteral("task.deadline")), summarySection_), 2, 0);
    summaryGrid->addWidget(deadlineLabel_, 2, 1);
    summaryGrid->addWidget(new QLabel(localization_.text(QStringLiteral("common.meta.created")), summarySection_), 3, 0);
    summaryGrid->addWidget(createdLabel_, 3, 1);
    summaryGrid->addWidget(new QLabel(localization_.text(QStringLiteral("common.meta.updated")), summarySection_), 4, 0);
    summaryGrid->addWidget(updatedLabel_, 4, 1);
    summarySection_->contentLayout()->addLayout(summaryGrid);

    descriptionSection_->setTitle(localization_.text(QStringLiteral("common.label.description")));
    descriptionSection_->contentLayout()->addWidget(descriptionLabel_);

    setSummaryWidget(summarySection_);
    addContentWidget(descriptionSection_);

    showTask(0);
}

void TaskDetailsPage::showTask(std::int64_t taskId)
{
    if (taskId <= 0)
    {
        task_ = {};
        header_->setTitle(localization_.text(QStringLiteral("common.entity.task")));
        header_->setDescription(QString());
        summarySection_->setVisible(false);
        descriptionSection_->setVisible(false);
        editButton_->setVisible(false);
        deleteButton_->setVisible(false);
        return;
    }

    try
    {
        const auto task = taskService_.getTask(taskId);

        if (!task.has_value())
        {
            task_ = {};
            header_->setTitle(localization_.text(QStringLiteral("common.state.not_found.title")));
            header_->setDescription(localization_.text(QStringLiteral("common.state.not_found.description")));
            summarySection_->setVisible(false);
            descriptionSection_->setVisible(false);
            editButton_->setVisible(false);
            deleteButton_->setVisible(false);
            return;
        }

        task_ = task.value();
        QString parentName;

        if (task_.parentType == TaskParentType::Project)
        {
            const auto project = projectService_.getProject(task_.parentId);

            if (project.has_value())
                parentName = QString::fromUtf8(project->name.c_str());
        }
        else
        {
            const auto content = contentService_.getContent(task_.parentId);

            if (content.has_value())
                parentName = QString::fromUtf8(content->name.c_str());
        }

        if (parentName.isEmpty())
            parentName = localization_.text(QStringLiteral("common.state.not_found.title"));

        if (task_.parentTaskId.has_value())
        {
            parentName += QStringLiteral(" / ");
            parentName += localization_.text(QStringLiteral("task.parent_task"));
            parentName += QStringLiteral(" #");
            parentName += QString::number(task_.parentTaskId.value());
        }

        header_->setTitle(QString::fromUtf8(task_.name.c_str()));
        header_->setDescription(QString::fromUtf8(task_.description.c_str()));

        parentLabel_->setText(parentName);
        statusLabel_->setText(taskStatusText(task_.status, localization_));

        deadlineLabel_->setText(task_.deadlineAt.empty() ? QStringLiteral("—") : QString::fromUtf8(task_.deadlineAt.c_str()));

        createdLabel_->setText(QString::fromUtf8(task_.createdAt.c_str()));
        updatedLabel_->setText(QString::fromUtf8(task_.updatedAt.c_str()));
        descriptionLabel_->setText(QString::fromUtf8(task_.description.c_str()));

        summarySection_->setVisible(true);
        descriptionSection_->setVisible(!task_.description.empty());
        editButton_->setVisible(true);
        deleteButton_->setVisible(true);
    }
    catch (const std::exception &)
    {
        task_ = {};
        header_->setTitle(localization_.text(QStringLiteral("common.state.error.title")));
        header_->setDescription(localization_.text(QStringLiteral("common.state.error.load")));
        summarySection_->setVisible(false);
        descriptionSection_->setVisible(false);
        editButton_->setVisible(false);
        deleteButton_->setVisible(false);
    }
}