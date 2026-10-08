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

ProjectEditorDialog::ProjectEditorDialog(ProjectService &projectService, LocalizationService &localization, QWidget *parent, std::optional<Project> project)
    : EditorDialog(
        project.has_value() ? EditorDialog::Mode::Edit : EditorDialog::Mode::Create,
        project.has_value() ? localization.text(QStringLiteral("common.dialog.edit")) : localization.text(QStringLiteral("project.create.title")),
        project.has_value() ? localization.text(QStringLiteral("common.action.save")) : localization.text(QStringLiteral("common.action.create")),
        localization.text(QStringLiteral("common.action.cancel")),
        parent
    ),
      projectService_(projectService),
      localization_(localization),
      projectId_(project.has_value() ? project->id : 0),
      nameEdit_(new QLineEdit(this)),
      descriptionEdit_(new QPlainTextEdit(this)),
      plannedReleaseEdit_(new QDateEdit(this)),
      contentTypeCombo_(nullptr),
      contentNameEdit_(nullptr)
{
    resize(560, 520);

    auto *nameField = new FormField(this);
    nameField->setLabel(localization_.text(QStringLiteral("common.label.name")));
    nameField->setField(nameEdit_);

    auto *descriptionField = new FormField(this);
    descriptionField->setLabel(localization_.text(QStringLiteral("common.label.description")));
    descriptionField->setField(descriptionEdit_);

    auto *releaseField = new FormField(this);
    releaseField->setLabel(localization_.text(QStringLiteral("project.release")));

    plannedReleaseEdit_->setCalendarPopup(true);
    plannedReleaseEdit_->setDate(QDate::currentDate());
    plannedReleaseEdit_->setDisplayFormat(QStringLiteral("dd.MM.yyyy"));

    releaseField->setField(plannedReleaseEdit_);

    contentLayout()->addWidget(nameField);
    contentLayout()->addWidget(descriptionField);
    contentLayout()->addWidget(releaseField);

    if (project.has_value())
    {
        nameEdit_->setText(QString::fromUtf8(project->name.c_str()));
        descriptionEdit_->setPlainText(QString::fromUtf8(project->description.c_str()));

        const QDate plannedReleaseDate = QDate::fromString(QString::fromUtf8(project->plannedReleaseAt.c_str()), Qt::ISODate);

        if (plannedReleaseDate.isValid())
        {
            plannedReleaseEdit_->setDate(plannedReleaseDate);
        }
    }
    else
    {
        auto *contentTypeField = new FormField(this);

        contentTypeField->setLabel(localization_.text(QStringLiteral("project.create.content_type")));
        contentTypeCombo_ = new QComboBox(this);
        contentTypeField->setField(contentTypeCombo_);

        auto *contentNameField = new FormField(this);

        contentNameField->setLabel(localization_.text(QStringLiteral("common.label.name")));
        contentNameEdit_ = new QLineEdit(this);
        contentNameField->setField(contentNameEdit_);

        contentLayout()->addWidget(contentTypeField);
        contentLayout()->addWidget(contentNameField);

        const auto contentTypes = projectService_.getContentTypes();

        for (const ContentType &contentType : contentTypes)
        {
            contentTypeCombo_->addItem(
                QString::fromUtf8(contentType.name.c_str()),
                QVariant::fromValue(contentType.id));
        }

        if (contentTypeCombo_->count() == 0)
        {
            setSaveEnabled(false);

            QMessageBox::warning(
                this,
                localization_.text(QStringLiteral("common.state.error.title")),
                localization_.text(QStringLiteral("content.type.none_available"))
            );
        }
    }
}

bool ProjectEditorDialog::save()
{
    if (nameEdit_->text().trimmed().isEmpty())
    {
        QMessageBox::warning(
            this,
            localization_.text(QStringLiteral("procommon.state.error.title")),
            localization_.text(QStringLiteral("common.validation.name_required"))
        );

        nameEdit_->setFocus();
        return false;
    }

    if (isEditMode())
    {
        try
        {
            return projectService_.updateProject(
                projectId_,
                nameEdit_->text().toStdString(),
                descriptionEdit_->toPlainText().toStdString(),
                plannedReleaseEdit_->date().toString(Qt::ISODate).toStdString()
            );
        }
        catch (const std::exception &)
        {
            QMessageBox::critical(
                this,
                localization_.text(QStringLiteral("common.state.error.title")),
                localization_.text(QStringLiteral("project.edit.error.description"))
            );

            return false;
        }
    }

    if (contentNameEdit_->text().trimmed().isEmpty())
    {
        QMessageBox::warning(
            this,
            localization_.text(QStringLiteral("common.state.error.title")),
            localization_.text(QStringLiteral("common.validation.name_required"))
        );

        contentNameEdit_->setFocus();
        return false;
    }

    if (contentTypeCombo_->currentIndex() < 0)
    {
        return false;
    }

    const std::int64_t contentTypeId = contentTypeCombo_->currentData().toLongLong();

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
            localization_.text(QStringLiteral("common.state.error.title")),
            localization_.text(QStringLiteral("project.create.error.description"))
        );

        return false;
    }
}