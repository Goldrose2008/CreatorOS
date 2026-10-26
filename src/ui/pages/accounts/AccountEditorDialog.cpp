#include "AccountEditorDialog.h"

#include <QComboBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QVariant>
#include <QVBoxLayout>
#include <QProxyStyle>
#include <QStyle>
#include <QStyleOption>

#include <exception>

#include "../../components/forms/FormField.h"
#include "../../localization/LocalizationService.h"
#include "../../../application/accounts/AccountService.h"
#include "../../../application/platforms/PlatformService.h"
#include "../../../domain/models/Platform.h"


namespace
{
    class DownwardComboBoxStyle final : public QProxyStyle
    {
    public:
        DownwardComboBoxStyle() : QProxyStyle(QStringLiteral("Fusion"))
        {
        }

        int styleHint(
            StyleHint hint,
            const QStyleOption *option = nullptr,
            const QWidget *widget = nullptr,
            QStyleHintReturn *returnData = nullptr
        ) const override
        {
            if (hint == SH_ComboBox_Popup)
                return 0;

            return QProxyStyle::styleHint(hint, option, widget, returnData);
        }
    };
}


AccountEditorDialog::AccountEditorDialog(
    AccountService &accountService,
    PlatformService &platformService,
    LocalizationService &localization,
    QWidget *parent,
    std::optional<Account> account
)
    : EditorDialog(
        account.has_value() ? Mode::Edit : Mode::Create,
        account.has_value() ? localization.text(QStringLiteral("common.dialog.edit.title")) : localization.text(QStringLiteral("account.create.title")),
        account.has_value() ? localization.text(QStringLiteral("common.action.save")) : localization.text(QStringLiteral("common.action.create")),
        localization.text(QStringLiteral("common.action.cancel")),
        parent
    ),
      accountService_(accountService),
      localization_(localization),
      accountId_(account.has_value() ? account->id : 0),
      platformCombo_(new QComboBox(this)),
      nameEdit_(new QLineEdit(this)),
      usernameEdit_(new QLineEdit(this)),
      profileUrlEdit_(new QLineEdit(this)),
      externalIdEdit_(new QLineEdit(this))
{
    resize(540, 480);

    auto *comboStyle = new DownwardComboBoxStyle();
    comboStyle->setParent(platformCombo_);
    platformCombo_->setStyle(comboStyle);
    platformCombo_->setMaxVisibleItems(10);

    auto *platformField = new FormField(this);
    platformField->setLabel(localization_.text(QStringLiteral("account.field.platform")));
    platformField->setField(platformCombo_);

    auto *nameField = new FormField(this);
    nameField->setLabel(localization_.text(QStringLiteral("common.label.name")));
    nameField->setField(nameEdit_);

    auto *usernameField = new FormField(this);
    usernameField->setLabel(localization_.text(QStringLiteral("account.field.username")));
    usernameField->setField(usernameEdit_);

    auto *profileUrlField = new FormField(this);
    profileUrlField->setLabel(localization_.text(QStringLiteral("account.field.profile_url")));
    profileUrlField->setField(profileUrlEdit_);

    auto *externalIdField = new FormField(this);
    externalIdField->setLabel(localization_.text(QStringLiteral("account.field.external_id")));
    externalIdField->setField(externalIdEdit_);

    nameEdit_->setPlaceholderText(localization_.text(QStringLiteral("account.field.name.placeholder")));
    usernameEdit_->setPlaceholderText(localization_.text(QStringLiteral("account.field.username.placeholder")));
    profileUrlEdit_->setPlaceholderText(localization_.text(QStringLiteral("account.field.profile_url.placeholder")));
    externalIdEdit_->setPlaceholderText(localization_.text(QStringLiteral("account.field.external_id.placeholder")));

    contentLayout()->addWidget(platformField);
    contentLayout()->addWidget(nameField);
    contentLayout()->addWidget(usernameField);
    contentLayout()->addWidget(profileUrlField);
    contentLayout()->addWidget(externalIdField);

    const std::vector<Platform> platforms = platformService.getPlatforms();

    for (const Platform &platform : platforms)
        platformCombo_->addItem(QString::fromUtf8(platform.name.c_str()), QVariant::fromValue<qlonglong>(platform.id));

    if (account.has_value())
    {
        nameEdit_->setText(QString::fromUtf8(account->name.c_str()));
        usernameEdit_->setText(QString::fromUtf8(account->username.c_str()));
        profileUrlEdit_->setText(QString::fromUtf8(account->profileUrl.c_str()));
        externalIdEdit_->setText(QString::fromUtf8(account->externalId.c_str()));

        const int index = platformCombo_->findData(QVariant::fromValue<qlonglong>(account->platformId));

        if (index >= 0)
            platformCombo_->setCurrentIndex(index);
    }

    if (platformCombo_->count() == 0)
    {
        setSaveEnabled(false);
        QMessageBox::warning(this, localization_.text(QStringLiteral("common.state.error.title")), localization_.text(QStringLiteral("account.platform.none_available")));
    }
}

bool AccountEditorDialog::save()
{
    if (nameEdit_->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, localization_.text(QStringLiteral("common.state.error.title")), localization_.text(QStringLiteral("common.validation.name_required")));
        nameEdit_->setFocus();
        return false;
    }

    if (platformCombo_->currentIndex() < 0)
        return false;

    const std::int64_t platformId = platformCombo_->currentData().toLongLong();

    try
    {
        if (isEditMode())
        {
            const bool updated = accountService_.updateAccount(
                accountId_,
                platformId,
                nameEdit_->text().trimmed().toStdString(),
                usernameEdit_->text().trimmed().toStdString(),
                profileUrlEdit_->text().trimmed().toStdString(),
                externalIdEdit_->text().trimmed().toStdString()
            );

            if (!updated)
                QMessageBox::warning(this, localization_.text(QStringLiteral("common.state.error.title")), localization_.text(QStringLiteral("account.save.error")));

            return updated;
        }

        accountService_.createAccount(
            platformId,
            nameEdit_->text().trimmed().toStdString(),
            usernameEdit_->text().trimmed().toStdString(),
            profileUrlEdit_->text().trimmed().toStdString(),
            externalIdEdit_->text().trimmed().toStdString()
        );

        return true;
    }
    catch (const std::exception &)
    {
        QMessageBox::critical(this, localization_.text(QStringLiteral("common.state.error.title")), localization_.text(QStringLiteral("account.save.error")));
        return false;
    }
}