
#include "PlatformsPage.h"

#include <QLabel>
#include <QPalette>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <exception>

#include "../../components/layout/PageHeader.h"
#include "../../components/lists/EntityList.h"
#include "../../components/states/EmptyState.h"
#include "../../components/states/ErrorState.h"
#include "../../components/states/LoadingState.h"
#include "../../localization/LocalizationService.h"
#include "../../style/Colors.h"
#include "../../style/Metrics.h"
#include "../../../application/platforms/PlatformService.h"
#include "../../../domain/models/Platform.h"

namespace
{
    QWidget *createPlatformItem(const Platform &platform, QWidget *parent    )
    {
        auto *item = new QWidget(parent);
        auto *layout = new QVBoxLayout(item);
        auto *nameLabel = new QLabel(QString::fromUtf8(platform.name.c_str()), item);
        auto *urlLabel = new QLabel(QString::fromUtf8(platform.websiteUrl.c_str()), item);

        layout->setContentsMargins(
            CreatorMetrics::SpacingLarge,
            CreatorMetrics::SpacingMedium,
            CreatorMetrics::SpacingLarge,
            CreatorMetrics::SpacingMedium
        );
        layout->setSpacing(CreatorMetrics::SpacingSmall);

        QFont nameFont = nameLabel->font();
        nameFont.setBold(true);
        nameLabel->setFont(nameFont);

        QPalette urlPalette = urlLabel->palette();
        urlPalette.setColor(QPalette::WindowText, CreatorColors::TextSecondary);
        urlLabel->setPalette(urlPalette);
        urlLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

        layout->addWidget(nameLabel);

        if (!platform.websiteUrl.empty())
            layout->addWidget(urlLabel);

        return item;
    }
}

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
            platformList_->addItem(createPlatformItem(platform, platformList_));

        stateStack_->setCurrentWidget(platformList_);
    }
    catch (const std::exception &)
    {
        stateStack_->setCurrentWidget(errorState_);
    }
}
