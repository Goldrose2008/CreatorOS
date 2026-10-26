
#pragma once

#include <optional>

#include "../../components/forms/EditorDialog.h"
#include "../../../domain/models/Platform.h"

class LocalizationService;
class PlatformService;
class QLineEdit;

class PlatformEditorDialog final : public EditorDialog
{
    Q_OBJECT

public:
    explicit PlatformEditorDialog(
        PlatformService &platformService,
        LocalizationService &localization,
        QWidget *parent = nullptr,
        std::optional<Platform> platform = std::nullopt
    );

protected:
    bool save() override;

private:
    PlatformService &platformService_;
    LocalizationService &localization_;
    std::int64_t platformId_ = 0;

    QLineEdit *nameEdit_;
    QLineEdit *websiteUrlEdit_;
};
