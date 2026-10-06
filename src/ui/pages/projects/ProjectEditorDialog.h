#pragma once

#include <QDialog>

#include "../../components/forms/EditorDialog.h"

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
    explicit ProjectEditorDialog(ProjectService &projectService, LocalizationService &localization, QWidget *parent = nullptr);

protected:
    bool save() override;

private:
    ProjectService &projectService_;
    LocalizationService &localization_;

    QLineEdit *nameEdit_;
    QPlainTextEdit *descriptionEdit_;
    QDateEdit *plannedReleaseEdit_;
    QComboBox *contentTypeCombo_;
    QLineEdit *contentNameEdit_;
};