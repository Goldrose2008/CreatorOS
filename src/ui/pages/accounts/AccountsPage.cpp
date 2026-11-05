#include "AccountsPage.h"

#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include <exception>
#include <unordered_map>
#include <vector>

#include "AccountEditorDialog.h"
#include "../../components/forms/ConfirmModal.h"
#include "../../components/layout/PageHeader.h"
#include "../../components/entity/EntityList.h"
#include "../../localization/LocalizationService.h"
#include "../../style/Metrics.h"
#include "../../../application/accounts/AccountService.h"
#include "../../../application/platforms/PlatformService.h"
#include "../../../domain/models/Account.h"
#include "../../../domain/models/Platform.h"

AccountsPage::AccountsPage(AccountService &accountService, PlatformService &platformService, LocalizationService &localization, QWidget *parent)
    : QWidget(parent),
      accountService_(accountService),
      platformService_(platformService),
      localization_(localization),
      accountList_(new EntityList(this))
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(CreatorMetrics::SpacingXLarge);

    auto *header = new PageHeader(this);
    header->setTitle(localization_.text(QStringLiteral("accounts")));
    header->setDescription(localization_.text(QStringLiteral("accounts.description")));

    auto *createButton = new QPushButton(localization_.text(QStringLiteral("accounts.create")), this);
    header->addAction(createButton);

    
    connect(
        createButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            try
            {
                const auto platforms = platformService_.getPlatforms();

                if (platforms.empty())
                {
                    QMessageBox::information(this, localization_.text(QStringLiteral("common.state.error.title")), localization_.text(QStringLiteral("account.platform.none_available")));
                    return;
                }

                AccountEditorDialog dialog(accountService_, platformService_, localization_, this);

                if (dialog.exec() == QDialog::Accepted)
                    reload();
            }
            catch (const std::exception &)
            {
                QMessageBox::critical(this, localization_.text(QStringLiteral("common.state.error.title")), localization_.text(QStringLiteral("common.state.error.load")));
            }
        }
    );


    accountList_->setLoadingTitle(localization_.text(QStringLiteral("common.state.loading")));
    accountList_->setEmptyState(localization_.text(QStringLiteral("common.state.empty.title")), localization_.text(QStringLiteral("accounts.empty.description")));
    accountList_->setErrorState(
        localization_.text(QStringLiteral("common.state.error.title")),
        localization_.text(QStringLiteral("common.state.error.load")),
        localization_.text(QStringLiteral("common.action.retry"))
    );

    layout->addWidget(header);
    layout->addWidget(accountList_, 1);

    connect(
        accountList_,
        &EntityList::retryRequested,
        this,
        &AccountsPage::reload
    );

    reload();
}

void AccountsPage::reload()
{
    accountList_->showLoading();
    accountList_->clearItems();

    try
    {
        const std::vector<Platform> platforms = platformService_.getPlatforms();
        std::unordered_map<std::int64_t, std::string> platformNames;

        for (const Platform &platform : platforms)
            platformNames[platform.id] = platform.name;

        const std::vector<Account> accounts = accountService_.getAccounts();

        if (accounts.empty())
        {
            accountList_->showEmpty();
            return;
        }

        for (const Account &account : accounts)
        {
            auto *item = new QWidget(accountList_);
            auto *itemLayout = new QHBoxLayout(item);
            auto *textLayout = new QVBoxLayout();

            itemLayout->setContentsMargins(
                CreatorMetrics::SpacingLarge,
                CreatorMetrics::SpacingMedium,
                CreatorMetrics::SpacingLarge,
                CreatorMetrics::SpacingMedium
            );
            itemLayout->setSpacing(CreatorMetrics::SpacingMedium);
            textLayout->setContentsMargins(0, 0, 0, 0);
            textLayout->setSpacing(CreatorMetrics::SpacingSmall);

            auto *nameLabel = new QLabel(QString::fromUtf8(account.name.c_str()), item);
            QFont nameFont = nameLabel->font();
            nameFont.setBold(true);
            nameLabel->setFont(nameFont);
            textLayout->addWidget(nameLabel);

            QString details = localization_.text(QStringLiteral("account.field.platform"));
            details += QStringLiteral(": ");

            const auto platformIt = platformNames.find(account.platformId);

            if (platformIt != platformNames.end())
                details += QString::fromUtf8(platformIt->second.c_str());
            else
                details += QStringLiteral("—");

            if (!account.username.empty())
            {
                details += QStringLiteral("  ·  ");
                details += localization_.text(QStringLiteral("account.field.username"));
                details += QStringLiteral(": ");
                details += QString::fromUtf8(account.username.c_str());
            }

            auto *detailsLabel = new QLabel(details, item);
            detailsLabel->setWordWrap(true);
            textLayout->addWidget(detailsLabel);

            if (!account.profileUrl.empty())
            {
                auto *urlLabel = new QLabel(QString::fromUtf8(account.profileUrl.c_str()), item);
                urlLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
                urlLabel->setWordWrap(true);
                textLayout->addWidget(urlLabel);
            }

            if (!account.externalId.empty())
            {
                auto *externalIdLabel = new QLabel(localization_.text(QStringLiteral("account.field.external_id")) + QStringLiteral(": ") + QString::fromUtf8(account.externalId.c_str()), item);
                externalIdLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
                externalIdLabel->setWordWrap(true);
                textLayout->addWidget(externalIdLabel);
            }

            itemLayout->addLayout(textLayout, 1);

            auto *editButton = new QPushButton(localization_.text(QStringLiteral("common.action.edit")), item);
            auto *deleteButton = new QPushButton(localization_.text(QStringLiteral("common.action.delete")), item);

            itemLayout->addWidget(editButton);
            itemLayout->addWidget(deleteButton);

            connect(
                editButton,
                &QPushButton::clicked,
                this,
                [this, account]()
                {
                    AccountEditorDialog dialog(accountService_, platformService_, localization_, this, account);

                    if (dialog.exec() == QDialog::Accepted)
                        reload();
                }
            );

            connect(
                deleteButton,
                &QPushButton::clicked,
                this,
                [this, account]()
                {
                    ConfirmModal::confirm(
                        this,
                        localization_.text(QStringLiteral("common.dialog.delete.title")),
                        localization_.text(QStringLiteral("common.dialog.delete.description")),
                        localization_.text(QStringLiteral("common.action.delete")),
                        localization_.text(QStringLiteral("common.action.cancel")),
                        [this, account]()
                        {
                            try
                            {
                                if (!accountService_.deleteAccount(account.id))
                                {
                                    QMessageBox::warning(this, localization_.text(QStringLiteral("common.state.error.title")), localization_.text(QStringLiteral("account.delete.error")));
                                    return;
                                }

                                reload();
                            }
                            catch (const std::exception &)
                            {
                                QMessageBox::critical(this, localization_.text(QStringLiteral("common.state.error.title")), localization_.text(QStringLiteral("account.delete.error")));
                            }
                        }
                    );
                }
            );

            accountList_->addItem(item);
        }

        accountList_->showList();
    }
    catch (const std::exception &)
    {
        accountList_->showError();
    }
}