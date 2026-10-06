#include "ProjectEditorDialog.h"

#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QVBoxLayout>

#include "../../../application/projects/ProjectService.h"
#include "../../../domain/models/ContentType.h"
#include "../../components/forms/FormField.h"
#include "../../localization/LocalizationService.h"

ProjectEditorDialog::ProjectEditorDialog(ProjectService &projectService, LocalizationService &localization, QWidget *parent)
    : EditorDialog(
        localization.text(QStringLiteral("project.create.title")),
        localization.text(QStringLiteral("project.create.save")),
        localization.text(QStringLiteral("project.create.cancel")),
        parent
    ),
      projectService_(projectService),
      localization_(localization),
      nameEdit_(new QLineEdit(this)),
      descriptionEdit_(new QPlainTextEdit(this)),
      plannedReleaseEdit_(new QDateEdit(this)),
      contentTypeCombo_(new QComboBox(this)),
      contentNameEdit_(new QLineEdit(this))
{
    resize(560, 520);

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

    contentLayout()->addWidget(nameField);
    contentLayout()->addWidget(descriptionField);
    contentLayout()->addWidget(releaseField);
    contentLayout()->addWidget(contentTypeField);
    contentLayout()->addWidget(contentNameField);

    const auto contentTypes = projectService_.getContentTypes();

    for (const ContentType &contentType : contentTypes)
    {
        contentTypeCombo_->addItem(
            QString::fromUtf8(contentType.name.c_str()),
            QVariant::fromValue(contentType.id)
        );
    }

    if (contentTypeCombo_->count() == 0)
    {
        setSaveEnabled(false);

        QMessageBox::warning(
            this,
            localization_.text(QStringLiteral("project.create.error.title")),
            localization_.text(QStringLiteral("project.create.no_content_types"))
        );
    }
}

bool ProjectEditorDialog::save()
{
    if (nameEdit_->text().trimmed().isEmpty())
    {
        QMessageBox::warning(
            this,
            localization_.text(QStringLiteral("project.create.error.title")),
            localization_.text(QStringLiteral("project.create.name_required"))
        );

        nameEdit_->setFocus();
        return false;
    }

    if (contentNameEdit_->text().trimmed().isEmpty())
    {
        QMessageBox::warning(
            this,
            localization_.text(QStringLiteral("project.create.error.title")),
            localization_.text(QStringLiteral("project.create.content_name_required"))
        );

        contentNameEdit_->setFocus();
        return false;
    }

    if (contentTypeCombo_->currentIndex() < 0)
    {
        return false;
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

        return true;
    }
    catch (const std::exception &)
    {
        QMessageBox::critical(
            this,
            localization_.text(QStringLiteral("project.create.error.title")),
            localization_.text(QStringLiteral("project.create.error.description"))
        );

        return false;
    }
}