#include "ContentEditorDialog.h"

#include <QComboBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QVBoxLayout>

#include "../../../application/content/ContentService.h"
#include "../../../application/projects/ProjectService.h"
#include "../../../domain/models/ContentType.h"
#include "../../components/forms/FormField.h"
#include "../../localization/LocalizationService.h"

ContentEditorDialog::ContentEditorDialog(
    ContentService &contentService,
    ProjectService &projectService,
    LocalizationService &localization,
    QWidget *parent,
    std::int64_t projectId,
    std::optional<Content> content
)
    : EditorDialog(
        content.has_value() ? EditorDialog::Mode::Edit : EditorDialog::Mode::Create,
        content.has_value() ? localization.text(QStringLiteral("content.edit.title")) : localization.text(QStringLiteral("content.create.title")),
        content.has_value() ? localization.text(QStringLiteral("content.edit.save")) : localization.text(QStringLiteral("content.create.save")),
        localization.text(QStringLiteral("content.create.cancel")),
        parent
    ),
      contentService_(contentService),
      projectService_(projectService),
      localization_(localization),
      projectId_(content.has_value() ? content->projectId : projectId),
      contentId_(content.has_value() ? content->id : 0),
      contentTypeCombo_(new QComboBox(this)),
      nameEdit_(new QLineEdit(this)),
      descriptionEdit_(new QPlainTextEdit(this)),
      prioritySpin_(new QSpinBox(this))
{
    resize(560, 520);

    auto *contentTypeField = new FormField(this);
    contentTypeField->setLabel(localization_.text(QStringLiteral("content.create.type")));
    contentTypeField->setField(contentTypeCombo_);

    auto *nameField = new FormField(this);
    nameField->setLabel(localization_.text(QStringLiteral("content.create.name")));
    nameField->setField(nameEdit_);

    auto *descriptionField = new FormField(this);
    descriptionField->setLabel(localization_.text(QStringLiteral("content.create.description")));
    descriptionField->setField(descriptionEdit_);

    auto *priorityField = new FormField(this);
    priorityField->setLabel(localization_.text(QStringLiteral("content.create.priority")));
    priorityField->setField(prioritySpin_);

    prioritySpin_->setRange(0, 100);

    contentLayout()->addWidget(contentTypeField);
    contentLayout()->addWidget(nameField);
    contentLayout()->addWidget(descriptionField);
    contentLayout()->addWidget(priorityField);

    const auto contentTypes = projectService_.getContentTypes();

    for (const ContentType &contentType : contentTypes)
    {
        contentTypeCombo_->addItem(QString::fromUtf8(contentType.name.c_str()), QVariant::fromValue(contentType.id));
    }

    if (content.has_value())
    {
        nameEdit_->setText(QString::fromUtf8(content->name.c_str()));
        descriptionEdit_->setPlainText(QString::fromUtf8(content->description.c_str()));
        prioritySpin_->setValue(content->priority);

        const int index = contentTypeCombo_->findData(QVariant::fromValue(content->contentTypeId));

        if (index >= 0)
        {
            contentTypeCombo_->setCurrentIndex(index);
        }
    }

    if (contentTypeCombo_->count() == 0)
    {
        setSaveEnabled(false);

        QMessageBox::warning(
            this,
            localization_.text(QStringLiteral("content.error.title")),
            localization_.text(QStringLiteral("content.create.no_types"))
        );
    }
}

bool ContentEditorDialog::save()
{
    if (nameEdit_->text().trimmed().isEmpty())
    {
        QMessageBox::warning(
            this,
            localization_.text(QStringLiteral("content.error.title")),
            localization_.text(QStringLiteral("content.create.name_required"))
        );

        nameEdit_->setFocus();

        return false;
    }

    if (contentTypeCombo_->currentIndex() < 0){return false;}

    const std::int64_t contentTypeId = contentTypeCombo_->currentData().toLongLong();

    try
    {
        if (isEditMode())
        {
            return contentService_.updateContent(
                contentId_,
                contentTypeId,
                nameEdit_->text().toStdString(),
                descriptionEdit_->toPlainText().toStdString(),
                prioritySpin_->value()
            );
        }

        contentService_.createAdditionalContent(
            projectId_,
            contentTypeId,
            nameEdit_->text().toStdString(),
            descriptionEdit_->toPlainText().toStdString(),
            prioritySpin_->value()
        );

        return true;
    }
    catch (const std::exception &)
    {
        QMessageBox::critical(
            this,
            localization_.text(QStringLiteral("content.error.title")),
            localization_.text(QStringLiteral("content.save.error"))
        );

        return false;
    }
}