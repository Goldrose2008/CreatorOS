#pragma once

#include <QDialog>

class QComboBox;
class QDateEdit;
class QDialogButtonBox;
class QLineEdit;
class QPlainTextEdit;
class LocalizationService;
class ProjectService;

class ProjectEditorDialog final : public QDialog
{
    Q_OBJECT

public:
    explicit ProjectEditorDialog(ProjectService &projectService, LocalizationService &localization, QWidget *parent = nullptr);

private slots:
    void saveProject();

private:
    ProjectService &projectService_;
    LocalizationService &localization_;

    QLineEdit *nameEdit_;
    QPlainTextEdit *descriptionEdit_;
    QDateEdit *plannedReleaseEdit_;
    QComboBox *contentTypeCombo_;
    QLineEdit *contentNameEdit_;
    QDialogButtonBox *buttonBox_;
};