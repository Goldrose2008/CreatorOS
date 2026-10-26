
#include "PlatformsPage.h"

#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPalette>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <exception>

#include "PlatformEditorDialog.h"
#include "../../components/forms/ConfirmModal.h"
#include "../../components/layout/PageHeader.h"
#include "../../components/entity/EntityList.h"
#include "../../components/states/EmptyState.h"
#include "../../components/states/ErrorState.h"
#include "../../components/states/LoadingState.h"
#include "../../localization/LocalizationService.h"
#include "../../style/Colors.h"
#include "../../style/Metrics.h"
#include "../../../application/platforms/PlatformService.h"
#include "../../../domain/models/Platform.h"

PlatformsPage::PlatformsPage(PlatformService &platformService, LocalizationService &localization, QWidget *parent
)
    : QWidget(parent),
      platformService_(platformService),
      localization_(localization),
      platformList_(new EntityList(this)),
      loadingState_(new LoadingState(this)),
      emptyState_(new EmptyState(this)),
      errorState_(new ErrorState(this)),
      stateStack_(new QStackedWidget(this))
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

    loadingState_->setTitle(localization_.text(QStringLiteral("common.state.loading")));

    emptyState_->setTitle(localization_.text(QStringLiteral("common.state.empty.title")));
    emptyState_->setDescription(localization_.text(QStringLiteral("platforms.empty.description")));

    errorState_->setTitle(localization_.text(QStringLiteral("common.state.error.title")));
    errorState_->setDescription(localization_.text(QStringLiteral("common.state.error.load")));
    errorState_->setRetryText(localization_.text(QStringLiteral("common.action.retry")));

    stateStack_->addWidget(platformList_);
    stateStack_->addWidget(loadingState_);
    stateStack_->addWidget(emptyState_);
    stateStack_->addWidget(errorState_);

    layout->addWidget(header);
    layout->addWidget(stateStack_, 1);

    connect(
        errorState_,
        &ErrorState::retryRequested,
        this,
        &PlatformsPage::reload
    );

    reload();
}

void PlatformsPage::reload()
{
    stateStack_->setCurrentWidget(loadingState_);
    platformList_->clearItems();

    try
    {
        const std::vector<Platform> platforms = platformService_.getPlatforms();

        if (platforms.empty())
        {
            stateStack_->setCurrentWidget(emptyState_);
            return;
        }

        for (const Platform &platform : platforms)
        {
            auto *item = new QWidget(platformList_);
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

            auto *nameLabel = new QLabel(QString::fromUtf8(platform.name.c_str()), item);
            QFont nameFont = nameLabel->font();
            nameFont.setBold(true);
            nameLabel->setFont(nameFont);

            textLayout->addWidget(nameLabel);

            if (!platform.websiteUrl.empty())
            {
                auto *urlLabel = new QLabel(QString::fromUtf8(platform.websiteUrl.c_str()), item);

                QPalette palette = urlLabel->palette();
                palette.setColor(QPalette::WindowText, CreatorColors::TextSecondary);
                urlLabel->setPalette(palette);
                urlLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

                textLayout->addWidget(urlLabel);
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
                                        platformService_.hasAccounts(platform.id)
                                            ? localization_.text(QStringLiteral("platform.delete.has_accounts"))
                                            : localization_.text(QStringLiteral("platform.delete.error"))
                                    );
                                    return;
                                }

                                reload();
                            }
                            catch (const std::exception &)
                            {
                                QMessageBox::critical(this, localization_.text(QStringLiteral("common.state.error.title")), localization_.text(QStringLiteral("platform.delete.error")));
                            }
                        }
                    );
                }
            );

            platformList_->addItem(item);
        }

        stateStack_->setCurrentWidget(platformList_);
    }
    catch (const std::exception &)
    {
        stateStack_->setCurrentWidget(errorState_);
    }
}
