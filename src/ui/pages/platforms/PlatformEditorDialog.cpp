
#include "PlatformEditorDialog.h"

#include <QLineEdit>
#include <QMessageBox>
#include <QVBoxLayout>
#include <exception>

#include "../../components/forms/FormField.h"
#include "../../localization/LocalizationService.h"
#include "../../../application/platforms/PlatformService.h"

PlatformEditorDialog::PlatformEditorDialog(
    PlatformService &platformService,
    LocalizationService &localization,
    QWidget *parent,
    std::optional<Platform> platform
)
    : EditorDialog(
        platform.has_value() ? Mode::Edit : Mode::Create,
        platform.has_value() ? localization.text(QStringLiteral("common.dialog.edit.title")) : localization.text(QStringLiteral("platform.create.title")),
        platform.has_value() ? localization.text(QStringLiteral("common.action.save")) : localization.text(QStringLiteral("common.action.create")),
        localization.text(QStringLiteral("common.action.cancel")),
        parent
    ),
      platformService_(platformService),
      localization_(localization),
      platformId_(platform.has_value() ? platform->id : 0),
      nameEdit_(new QLineEdit(this)),
      websiteUrlEdit_(new QLineEdit(this))
{
    resize(520, 300);

    nameEdit_->setPlaceholderText(localization_.text(QStringLiteral("platform.field.name.placeholder")));
    websiteUrlEdit_->setPlaceholderText(localization_.text(QStringLiteral("platform.field.url.placeholder")));

    auto *nameField = new FormField(this);
    nameField->setLabel(localization_.text(QStringLiteral("common.label.name")));
    nameField->setField(nameEdit_);

    auto *urlField = new FormField(this);
    urlField->setLabel(localization_.text(QStringLiteral("platform.field.url")));
    urlField->setField(websiteUrlEdit_);

    contentLayout()->addWidget(nameField);
    contentLayout()->addWidget(urlField);

    if (platform.has_value())
    {
        nameEdit_->setText(QString::fromUtf8(platform->name.c_str()));
        websiteUrlEdit_->setText(QString::fromUtf8(platform->websiteUrl.c_str()));
    }
}

bool PlatformEditorDialog::applyChanges()
{
    if (!platformService_.isPlatformNameValid(nameEdit_->text().toStdString()))
    {
        QMessageBox::warning(this, localization_.text(QStringLiteral("common.state.error.title")), localization_.text(QStringLiteral("common.validation.name_required")));
        nameEdit_->setFocus();
        return false;
    }

    try
    {
        Platform sourcePlatform;
        sourcePlatform.id = platformId_;
        sourcePlatform.name = nameEdit_->text().toStdString();
        sourcePlatform.websiteUrl = websiteUrlEdit_->text().toStdString();

        if (isEditMode())
        {
            const bool updated = platformService_.updatePlatform(sourcePlatform);

            if (!updated)
                QMessageBox::warning(this, localization_.text(QStringLiteral("common.state.error.title")), localization_.text(QStringLiteral("platform.save.error")));

            return updated;
        }

        platformService_.createPlatform(sourcePlatform);

        return true;
    }
    catch (const std::exception &)
    {
        QMessageBox::critical(this, localization_.text(QStringLiteral("common.state.error.title")), localization_.text(QStringLiteral("platform.save.error")));

        return false;
    }
}
