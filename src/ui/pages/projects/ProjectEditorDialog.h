#pragma once

#include <QDialog>
#include <optional>

#include "../../components/forms/EditorDialog.h"
#include "../../../domain/models/Project.h"

class QComboBox;
class QDateEdit;
class QLineEdit;
class QPlainTextEdit;
class LocalizationService;
class ProjectService;

class ProjectEditorDialog final : public EditorDialog
{
    Q_OBJECT

public:
    explicit ProjectEditorDialog(ProjectService &projectService, LocalizationService &localization, QWidget *parent = nullptr,std::optional<Project> project = std::nullopt);

protected:
    bool save() override;

private:
    ProjectService &projectService_;
    LocalizationService &localization_;

    std::int64_t projectId_ = 0;

    QLineEdit *nameEdit_;
    QPlainTextEdit *descriptionEdit_;
    QDateEdit *plannedReleaseEdit_;
    QComboBox *contentTypeCombo_;
    QLineEdit *contentNameEdit_;
};