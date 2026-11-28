#include "ObjectTypeEditorDialog.h"

#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QVBoxLayout>

#include <exception>

#include "../../components/forms/FormField.h"
#include "../../localization/LocalizationService.h"
#include "../../../application/object_types/ObjectTypeService.h"

ObjectTypeEditorDialog::ObjectTypeEditorDialog(
    ObjectTypeService &objectTypeService,
    LocalizationService &localization,
    QWidget *parent,
    std::optional<ObjectType> objectType
)
    : EditorDialog(
        objectType.has_value() ? Mode::Edit : Mode::Create,
        objectType.has_value() ? localization.text(QStringLiteral("common.dialog.edit.title")) : localization.text(QStringLiteral("object_types.create.title")),
        objectType.has_value() ? localization.text(QStringLiteral("common.action.save")) : localization.text(QStringLiteral("common.action.create")),
        localization.text(QStringLiteral("common.action.cancel")),
        parent
    ),
      objectTypeService_(objectTypeService),
      localization_(localization),
      objectTypeId_(objectType.has_value() ? objectType->id : 0),
      nameEdit_(new QLineEdit(this)),
      descriptionEdit_(new QPlainTextEdit(this))
{
    resize(520, 360);

    auto *nameField = new FormField(this);
    nameField->setLabel(localization_.text(QStringLiteral("common.label.name")));
    nameField->setField(nameEdit_);

    auto *descriptionField = new FormField(this);
    descriptionField->setLabel(localization_.text(QStringLiteral("common.label.description")));
    descriptionField->setField(descriptionEdit_);
    descriptionEdit_->setMinimumHeight(120);

    contentLayout()->addWidget(nameField);
    contentLayout()->addWidget(descriptionField);

    if (objectType.has_value())
    {
        nameEdit_->setText(QString::fromUtf8(objectType->name.c_str()));

        if (objectType->description.has_value())
            descriptionEdit_->setPlainText(QString::fromUtf8(objectType->description->c_str()));
    }
}

bool ObjectTypeEditorDialog::applyChanges()
{
    const std::string name = nameEdit_->text().toStdString();

    if (nameEdit_->text().trimmed().isEmpty())
    {
        QMessageBox::warning(
            this,
            localization_.text(QStringLiteral("common.state.error.title")),
            localization_.text(QStringLiteral("common.validation.name_required"))
        );

        nameEdit_->setFocus();
        return false;
    }

    const std::optional<std::int64_t> excludeId = objectTypeId_ > 0 ? std::optional<std::int64_t>(objectTypeId_) : std::nullopt;

    if (!objectTypeService_.isObjectTypeNameValid(name, excludeId))
    {
        QMessageBox::warning(
            this,
            localization_.text(QStringLiteral("common.state.error.title")),
            localization_.text(QStringLiteral("object_types.validation.name_exists"))
        );

        nameEdit_->setFocus();
        return false;
    }

    ObjectType sourceObjectType;
    sourceObjectType.id = objectTypeId_;
    sourceObjectType.name = name;
    sourceObjectType.description = descriptionEdit_->toPlainText().toStdString();

    try
    {
        if (isEditMode())
        {
            if (!objectTypeService_.updateObjectType(sourceObjectType))
            {
                QMessageBox::warning(
                    this,
                    localization_.text(QStringLiteral("common.state.error.title")),
                    localization_.text(QStringLiteral("object_types.save.error"))
                );

                return false;
            }
        }
        else
        {
            objectTypeService_.createObjectType(sourceObjectType);
        }

        return true;
    }
    catch (const std::exception &)
    {
        QMessageBox::critical(
            this,
            localization_.text(QStringLiteral("common.state.error.title")),
            localization_.text(QStringLiteral("object_types.save.error"))
        );

        return false;
    }
}