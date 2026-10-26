#pragma once

#include <cstdint>
#include <optional>

#include "../../components/forms/EditorDialog.h"
#include "../../../domain/models/Account.h"

class AccountService;
class PlatformService;
class LocalizationService;
class QComboBox;
class QLineEdit;

class AccountEditorDialog final : public EditorDialog
{
    Q_OBJECT

public:
    explicit AccountEditorDialog(
        AccountService &accountService,
        PlatformService &platformService,
        LocalizationService &localization,
        QWidget *parent = nullptr,
        std::optional<Account> account = std::nullopt
    );

protected:
    bool save() override;

private:
    AccountService &accountService_;
    LocalizationService &localization_;
    std::int64_t accountId_ = 0;

    QComboBox *platformCombo_;
    QLineEdit *nameEdit_;
    QLineEdit *usernameEdit_;
    QLineEdit *profileUrlEdit_;
    QLineEdit *externalIdEdit_;
};