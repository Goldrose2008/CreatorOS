#include "ProjectEditorDialog.h"

#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QPushButton>

#include "../../../application/projects/ProjectService.h"
#include "../../../domain/models/ContentType.h"
#include "../../components/forms/FormField.h"
#include "../../localization/LocalizationService.h"

ProjectEditorDialog::ProjectEditorDialog(ProjectService &projectService, LocalizationService &localization, QWidget *parent)
    : QDialog(parent),
      projectService_(projectService),
      localization_(localization),
      nameEdit_(new QLineEdit(this)),
      descriptionEdit_(new QPlainTextEdit(this)),
      plannedReleaseEdit_(new QDateEdit(this)),
      contentTypeCombo_(new QComboBox(this)),
      contentNameEdit_(new QLineEdit(this)),
      buttonBox_(new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Save, this))
{
    setWindowTitle(localization_.text(QStringLiteral("project.create.title")));
    resize(560, 520);

    auto *layout = new QVBoxLayout(this);

    auto *nameField = new FormField(this);
    nameField->setLabel(localization_.text(QStringLiteral("project.create.name")));
    nameField->setField(nameEdit_);

    auto *descriptionField = new FormField(this);
    descriptionField->setLabel(localization_.text(QStringLiteral("project.create.description")));
    descriptionField->setField(descriptionEdit_);

    auto *releaseField = new FormField(this);
    releaseField->setLabel(localization_.text(QStringLiteral("project.create.release")));

    plannedReleaseEdit_->setCalendarPopup(true);
    plannedReleaseEdit_->setDate(QDate::currentDate());
    plannedReleaseEdit_->setDisplayFormat(QStringLiteral("dd.MM.yyyy"));

    releaseField->setField(plannedReleaseEdit_);

    auto *contentTypeField = new FormField(this);
    contentTypeField->setLabel(localization_.text(QStringLiteral("project.create.content_type")));
    contentTypeField->setField(contentTypeCombo_);

    auto *contentNameField = new FormField(this);
    contentNameField->setLabel(localization_.text(QStringLiteral("project.create.content_name")));
    contentNameField->setField(contentNameEdit_);

    layout->addWidget(nameField);
    layout->addWidget(descriptionField);
    layout->addWidget(releaseField);
    layout->addWidget(contentTypeField);
    layout->addWidget(contentNameField);
    layout->addStretch();
    layout->addWidget(buttonBox_);

    const auto contentTypes = projectService_.getContentTypes();

    for (const ContentType &contentType : contentTypes)
    {
        contentTypeCombo_->addItem(
            QString::fromUtf8(contentType.name.c_str()),
            QVariant::fromValue(contentType.id)
        );
    }

    buttonBox_->button(QDialogButtonBox::Cancel)->setText(localization_.text(QStringLiteral("project.create.cancel")));
    buttonBox_->button(QDialogButtonBox::Save)->setText(localization_.text(QStringLiteral("project.create.save")));

    connect(
        buttonBox_,
        &QDialogButtonBox::accepted,
        this,
        &ProjectEditorDialog::saveProject
    );

    connect(
        buttonBox_,
        &QDialogButtonBox::rejected,
        this,
        &ProjectEditorDialog::reject
    );

    if (contentTypeCombo_->count() == 0)
    {
        buttonBox_->button(QDialogButtonBox::Save)->setEnabled(false);

        QMessageBox::warning(
            this,
            localization_.text(QStringLiteral("project.create.error.title")),
            localization_.text(QStringLiteral("project.create.no_content_types"))
        );
    }
}

void ProjectEditorDialog::saveProject()
{
    if (nameEdit_->text().trimmed().isEmpty())
    {
        QMessageBox::warning(
            this,
            localization_.text(QStringLiteral("project.create.error.title")),
            localization_.text(QStringLiteral("project.create.name_required"))
        );

        nameEdit_->setFocus();
        return;
    }

    if (contentNameEdit_->text().trimmed().isEmpty())
    {
        QMessageBox::warning(
            this,
            localization_.text(QStringLiteral("project.create.error.title")),
            localization_.text(QStringLiteral("project.create.content_name_required"))
        );

        contentNameEdit_->setFocus();
        return;
    }

    if (contentTypeCombo_->currentIndex() < 0)
    {
        return;
    }

    const std::int64_t contentTypeId =
        contentTypeCombo_->currentData().toLongLong();

    try
    {
        projectService_.createProject(
            nameEdit_->text().toStdString(),
            descriptionEdit_->toPlainText().toStdString(),
            plannedReleaseEdit_->date().toString(Qt::ISODate).toStdString(),
            contentTypeId,
            contentNameEdit_->text().toStdString()
        );

        accept();
    }
    catch (const std::exception &)
    {
        QMessageBox::critical(
            this,
            localization_.text(QStringLiteral("project.create.error.title")),
            localization_.text(QStringLiteral("project.create.error.description"))
        );
    }
}