#include "AccountsPage.h"

#include <QAbstractTextDocumentLayout>
#include <QDialog>
#include <QFrame>
#include <QLabel>
#include <QMessageBox>
#include <QPalette>
#include <QPushButton>
#include <QTextEdit>
#include <QTextDocument>
#include <QTextOption>
#include <QtMath>
#include <QVBoxLayout>

#include <exception>
#include <unordered_map>
#include <vector>

#include "AccountEditorDialog.h"
#include "../../components/entity/EntityCard.h"
#include "../../components/entity/EntityList.h"
#include "../../components/forms/ConfirmModal.h"
#include "../../components/layout/PageHeader.h"
#include "../../localization/LocalizationService.h"
#include "../../style/Colors.h"
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
      accountList_(new EntityList(localization_, this))
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

    accountList_->setEmptyState(QString(), localization_.text(QStringLiteral("accounts.empty.description")));

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

    try
    {
        const std::vector<Platform> platforms = platformService_.getPlatforms();
        std::unordered_map<std::int64_t, std::string> platformNames;

        for (const Platform &platform : platforms)
            platformNames[platform.id] = platform.name;

        const std::vector<Account> accounts = accountService_.getAccounts();

        accountList_->setItems(
            accounts,
            [this, &platformNames](const Account &account) -> QWidget *
            {

                auto *item = new EntityCard(localization_, accountList_);
                item->setTitle(QString::fromUtf8(account.name.c_str()));

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
                item->addContentWidget(detailsLabel);

                if (!account.profileUrl.empty())
                {
                    auto *urlText = new QTextEdit(item);
                    urlText->setPlainText(QString::fromUtf8(account.profileUrl.c_str()));
                    urlText->document()->setDocumentMargin(0);
                    urlText->setReadOnly(true);
                    urlText->setFrameShape(QFrame::NoFrame);
                    urlText->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    urlText->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    urlText->setLineWrapMode(QTextEdit::WidgetWidth);
                    urlText->setWordWrapMode(QTextOption::WrapAnywhere);
                    urlText->setTextInteractionFlags(Qt::TextSelectableByMouse);
                    urlText->setMinimumWidth(0);
                    urlText->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);

                    QPalette palette = urlText->palette();
                    palette.setColor(QPalette::Text, CreatorColors::TextSecondary);
                    urlText->setPalette(palette);

                    connect(
                        urlText->document()->documentLayout(),
                        &QAbstractTextDocumentLayout::documentSizeChanged,
                        urlText,
                        [urlText](const QSizeF &documentSize)
                        {
                            const int textHeight = qCeil(documentSize.height());
                            urlText->setFixedHeight(textHeight + 2);
                        }
                    );

                    item->addContentWidget(urlText);
                }

                if (!account.externalId.empty())
                {
                    auto *externalIdLabel = new QLabel(localization_.text(QStringLiteral("account.field.external_id")) + QStringLiteral(": ") + QString::fromUtf8(account.externalId.c_str()), item);

                    externalIdLabel->setWordWrap(true);
                    externalIdLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
                    item->addContentWidget(externalIdLabel);
                }

                auto *editButton = new QPushButton(localization_.text(QStringLiteral("common.action.edit")), item);
                auto *deleteButton = new QPushButton(localization_.text(QStringLiteral("common.action.delete")), item);

                item->addAction(editButton);
                item->addAction(deleteButton);

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
                return item;
            }
        );
    }
    catch (const std::exception &)
    {
        accountList_->showError();
    }
}