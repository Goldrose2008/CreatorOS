
#include "PlatformsPage.h"

#include <QAbstractScrollArea>
#include <QDialog>
#include <QFrame>
#include <QLabel>
#include <QMessageBox>
#include <QPalette>
#include <QPushButton>
#include <QSizePolicy>
#include <QTextEdit>
#include <QTextOption>
#include <QVBoxLayout>
#include <exception>

#include "PlatformEditorDialog.h"
#include "../../components/forms/ConfirmModal.h"
#include "../../components/entity/EntityCard.h"
#include "../../components/entity/EntityList.h"
#include "../../components/layout/PageHeader.h"
#include "../../localization/LocalizationService.h"
#include "../../style/Colors.h"
#include "../../style/Metrics.h"
#include "../../../application/platforms/PlatformService.h"
#include "../../../domain/models/Platform.h"

PlatformsPage::PlatformsPage(PlatformService &platformService, LocalizationService &localization, QWidget *parent)
    : QWidget(parent),
      platformService_(platformService),
      localization_(localization),
      platformList_(new EntityList(localization_, this))
{
    auto *layout = new QVBoxLayout(this);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(CreatorMetrics::SpacingXLarge);

    auto *header = new PageHeader(this);
    header->setTitle(localization_.text(QStringLiteral("platforms")));
    header->setDescription(localization_.text(QStringLiteral("platforms.description")));

    auto *createButton = new QPushButton(localization_.text(QStringLiteral("platforms.create")), this);
    header->addAction(createButton);

    connect(
        createButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            PlatformEditorDialog dialog(platformService_, localization_, this);
            if (dialog.exec() == QDialog::Accepted)
                reload();
        }
    );

    platformList_->setEmptyState(QString(), localization_.text(QStringLiteral("platforms.empty.description")));

    layout->addWidget(header);
    layout->addWidget(platformList_, 1);

    connect(
        platformList_,
        &EntityList::retryRequested,
        this,
        &PlatformsPage::reload
    );

    reload();
}

void PlatformsPage::reload()
{
    platformList_->showLoading();

    try
    {
        const std::vector<Platform> platforms = platformService_.getPlatforms();

        platformList_->setItems(
            platforms,
            [this](const Platform &platform) -> QWidget *
            {
                auto *item = new EntityCard(localization_, platformList_);
                item->setTitle(QString::fromUtf8(platform.name.c_str()));

                if (!platform.websiteUrl.empty())
                {
                    auto *urlText = new QTextEdit(item);
                    urlText->setPlainText(QString::fromUtf8(platform.websiteUrl.c_str()));
                    urlText->setReadOnly(true);
                    urlText->setFrameShape(QFrame::NoFrame);
                    urlText->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    urlText->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    urlText->setLineWrapMode(QTextEdit::WidgetWidth);
                    urlText->setWordWrapMode(QTextOption::WrapAnywhere);
                    urlText->setTextInteractionFlags(Qt::TextSelectableByMouse);
                    urlText->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
                    urlText->setMinimumWidth(0);
                    urlText->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);

                    QPalette palette = urlText->palette();
                    palette.setColor(QPalette::Text, CreatorColors::TextSecondary);
                    urlText->setPalette(palette);

                    item->addContentWidget(urlText);
                }

                auto *editButton = new QPushButton(localization_.text(QStringLiteral("common.action.edit")), item);
                auto *deleteButton = new QPushButton(localization_.text(QStringLiteral("common.action.delete")), item);

                item->addAction(editButton);
                item->addAction(deleteButton);

                connect(
                    editButton,
                    &QPushButton::clicked,
                    this,
                    [this, platform]()
                    {
                        PlatformEditorDialog dialog(platformService_, localization_, this, platform);

                        if (dialog.exec() == QDialog::Accepted)
                            reload();
                    }
                );

                connect(
                    deleteButton,
                    &QPushButton::clicked,
                    this,
                    [this, platform]()
                    {
                        ConfirmModal::confirm(
                            this,
                            localization_.text(QStringLiteral("common.dialog.delete.title")),
                            localization_.text(QStringLiteral("common.dialog.delete.description")),
                            localization_.text(QStringLiteral("common.action.delete")),
                            localization_.text(QStringLiteral("common.action.cancel")),
                            [this, platform]()
                            {
                                try
                                {
                                    if (!platformService_.deletePlatform(platform.id))
                                    {
                                        QMessageBox::warning(
                                            this,
                                            localization_.text(QStringLiteral("common.state.error.title")),
                                            platformService_.hasAccounts(platform.id) ? localization_.text(QStringLiteral("platform.delete.has_accounts")) : localization_.text(QStringLiteral("platform.delete.error"))
                                        );
                                        return;
                                    }

                                    reload();
                                }
                                catch (const std::exception &)
                                {
                                    QMessageBox::critical(
                                        this,
                                        localization_.text(QStringLiteral("common.state.error.title")),
                                        localization_.text(QStringLiteral("platform.delete.error"))
                                    );
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
        platformList_->showError();
    }
}
