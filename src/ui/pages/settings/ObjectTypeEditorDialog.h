#pragma once

#include <optional>

#include "../../components/forms/EditorDialog.h"
#include "../../../domain/models/ObjectType.h"

class LocalizationService;
class ObjectTypeService;
class QLineEdit;
class QPlainTextEdit;

class ObjectTypeEditorDialog final : public EditorDialog
{
    Q_OBJECT

public:
    explicit ObjectTypeEditorDialog(
        ObjectTypeService &objectTypeService,
        LocalizationService &localization,
        QWidget *parent = nullptr,
        std::optional<ObjectType> objectType = std::nullopt
    );

protected:
    bool applyChanges() override;

private:
    ObjectTypeService &objectTypeService_;
    LocalizationService &localization_;

    std::int64_t objectTypeId_ = 0;

    QLineEdit *nameEdit_;
    QPlainTextEdit *descriptionEdit_;
};