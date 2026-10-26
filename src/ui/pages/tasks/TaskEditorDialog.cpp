#include "TaskEditorDialog.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QVBoxLayout>

#include <exception>

#include "../../components/forms/FormField.h"
#include "../../localization/LocalizationService.h"
#include "../../../application/content/ContentService.h"
#include "../../../application/projects/ProjectService.h"
#include "../../../application/tasks/TaskService.h"
#include "../../../domain/models/Content.h"
#include "../../../domain/models/Project.h"

TaskEditorDialog::TaskEditorDialog(
    TaskService &taskService,
    ProjectService &projectService,
    ContentService &contentService,
    LocalizationService &localization,
    QWidget *parent,
    std::optional<Task> task
)
    : EditorDialog(
        task.has_value() ? EditorDialog::Mode::Edit : EditorDialog::Mode::Create,
        task.has_value() ? localization.text(QStringLiteral("common.dialog.edit.title")) : localization.text(QStringLiteral("task.create.title")),
        task.has_value() ? localization.text(QStringLiteral("common.action.save")) : localization.text(QStringLiteral("common.action.create")),
        localization.text(QStringLiteral("common.action.cancel")),
        parent
    ),
      taskService_(taskService),
      projectService_(projectService),
      contentService_(contentService),
      localization_(localization),
      task_(task),
      parentTypeCombo_(new QComboBox(this)),
      parentCombo_(new QComboBox(this)),
      nameEdit_(new QLineEdit(this)),
      descriptionEdit_(new QPlainTextEdit(this)),
      deadlineEnabledCheck_(new QCheckBox(localization.text(QStringLiteral("task.deadline.enabled")), this)),
      deadlineEdit_(new QDateEdit(this))
{
    resize(560, 600);

    parentTypeCombo_->addItem(localization_.text(QStringLiteral("common.entity.project")), static_cast<int>(TaskParentType::Project));
    parentTypeCombo_->addItem(localization_.text(QStringLiteral("common.entity.content")), static_cast<int>(TaskParentType::Content));

    auto *parentTypeField = new FormField(this);
    parentTypeField->setLabel(localization_.text(QStringLiteral("task.create.parent_type")));
    parentTypeField->setField(parentTypeCombo_);

    auto *parentField = new FormField(this);
    parentField->setLabel(localization_.text(QStringLiteral("task.create.parent")));
    parentField->setField(parentCombo_);

    auto *nameField = new FormField(this);
    nameField->setLabel(localization_.text(QStringLiteral("common.label.name")));
    nameField->setField(nameEdit_);

    auto *descriptionField = new FormField(this);
    descriptionField->setLabel(localization_.text(QStringLiteral("common.label.description")));
    descriptionField->setField(descriptionEdit_);

    auto *deadlineField = new FormField(this);
    deadlineField->setLabel(localization_.text(QStringLiteral("task.deadline")));
    deadlineField->setField(deadlineEdit_);
    deadlineEdit_->setCalendarPopup(true);
    deadlineEdit_->setDisplayFormat(QStringLiteral("dd.MM.yyyy"));
    deadlineEdit_->setDate(QDate::currentDate());
    deadlineEdit_->setEnabled(false);

    contentLayout()->addWidget(parentTypeField);
    contentLayout()->addWidget(parentField);
    contentLayout()->addWidget(nameField);
    contentLayout()->addWidget(descriptionField);
    contentLayout()->addWidget(deadlineEnabledCheck_);
    contentLayout()->addWidget(deadlineField);

    connect(
        deadlineEnabledCheck_,
        &QCheckBox::toggled,
        deadlineEdit_,
        &QDateEdit::setEnabled
    );

    if (task_.has_value())
    {
        const Task &value = task_.value();
        parentTypeCombo_->setCurrentIndex(parentTypeCombo_->findData(static_cast<int>(value.parentType)));
        nameEdit_->setText(QString::fromUtf8(value.name.c_str()));
        descriptionEdit_->setPlainText(QString::fromUtf8(value.description.c_str()));

        if (!value.deadlineAt.empty())
        {
            const QDate date = QDate::fromString(QString::fromUtf8(value.deadlineAt.c_str()), Qt::ISODate);

            if (date.isValid())
            {
                deadlineEdit_->setDate(date);
                deadlineEnabledCheck_->setChecked(true);
            }
        }

        parentTypeCombo_->setEnabled(false);
        parentCombo_->setEnabled(false);
    }

    connect(
        parentTypeCombo_,
        &QComboBox::currentIndexChanged,
        this,
        [this](int)
        {
            reloadParentChoices();
        });

    reloadParentChoices();

    if (task_.has_value())
        parentCombo_->setCurrentIndex(parentCombo_->findData(QVariant::fromValue<qlonglong>(static_cast<qlonglong>(task_->parentId))));
}

void TaskEditorDialog::reloadParentChoices()
{
    parentCombo_->clear();

    try
    {
        const auto parentType = static_cast<TaskParentType>(parentTypeCombo_->currentData().toInt());

        if (parentType == TaskParentType::Project)
        {
            const auto projects = projectService_.getProjects();

            for (const Project &project : projects)
                parentCombo_->addItem(QString::fromUtf8(project.name.c_str()), QVariant::fromValue<qlonglong>(static_cast<qlonglong>(project.id)));
        }
        else
        {
            const auto contents = contentService_.getAllContents();

            for (const Content &content : contents)
                parentCombo_->addItem(QString::fromUtf8(content.name.c_str()), QVariant::fromValue<qlonglong>(static_cast<qlonglong>(content.id)));
        }
    }
    catch (const std::exception &)
    {
        QMessageBox::critical(this, localization_.text(QStringLiteral("common.state.error.title")), localization_.text(QStringLiteral("common.state.error.load")));
    }

    setSaveEnabled(isEditMode() || parentCombo_->count() > 0);
}

bool TaskEditorDialog::save()
{
    if (nameEdit_->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, localization_.text(QStringLiteral("common.state.error.title")), localization_.text(QStringLiteral("common.validation.name_required")));
        nameEdit_->setFocus();
        return false;
    }

    if (!isEditMode() && parentCombo_->currentIndex() < 0)
    {
        QMessageBox::warning(this, localization_.text(QStringLiteral("common.state.error.title")), localization_.text(QStringLiteral("task.parent.none")));
        return false;
    }

    std::string deadlineAt;

    if (deadlineEnabledCheck_->isChecked())
        deadlineAt = deadlineEdit_->date().toString(Qt::ISODate).toStdString();

    try
    {
        if (isEditMode())
        {
            if (!taskService_.updateTask(task_->id, nameEdit_->text().toStdString(), descriptionEdit_->toPlainText().toStdString(), deadlineAt))
            {
                QMessageBox::critical(this, localization_.text(QStringLiteral("common.state.error.title")), localization_.text(QStringLiteral("task.save.error.description")));
                return false;
            }

            return true;
        }

        const auto parentType = static_cast<TaskParentType>(parentTypeCombo_->currentData().toInt());
        const std::int64_t parentId = parentCombo_->currentData().toLongLong();

        taskService_.createTask(
            parentType,
            parentId,
            std::nullopt,
            nameEdit_->text().toStdString(),
            descriptionEdit_->toPlainText().toStdString(),
            deadlineAt);

        return true;
    }
    catch (const std::exception &)
    {
        QMessageBox::critical(this, localization_.text(QStringLiteral("common.state.error.title")), localization_.text(QStringLiteral("task.save.error.description")));
        return false;
    }
}