#pragma once

#include <optional>

#include "../../components/forms/EditorDialog.h"
#include "../../../domain/models/Task.h"

class QCheckBox;
class QComboBox;
class QDateEdit;
class QLineEdit;
class QPlainTextEdit;

class ContentService;
class LocalizationService;
class ProjectService;
class TaskService;

class TaskEditorDialog final : public EditorDialog
{
    Q_OBJECT

public:
    explicit TaskEditorDialog(
        TaskService &taskService,
        ProjectService &projectService,
        ContentService &contentService,
        LocalizationService &localization,
        QWidget *parent = nullptr,
        std::optional<Task> task = std::nullopt
    );

protected:
    bool save() override;

private:
    void reloadParentChoices();

    TaskService &taskService_;
    ProjectService &projectService_;
    ContentService &contentService_;
    LocalizationService &localization_;

    std::optional<Task> task_;

    QComboBox *parentTypeCombo_;
    QComboBox *parentCombo_;
    QLineEdit *nameEdit_;
    QPlainTextEdit *descriptionEdit_;
    QCheckBox *deadlineEnabledCheck_;
    QDateEdit *deadlineEdit_;
};