#include "AppShell.h"

#include <QLabel>
#include <QPalette>
#include <QHBoxLayout>

#include "MenuComposer.h"
#include "Sidebar.h"
#include "Workspace.h"

#include "../localization/LocalizationService.h"
#include "../style/Colors.h"
#include "../style/Metrics.h"
#include "../navigation/NavigationController.h"
#include "../navigation/CurrentPage.h"

namespace
{
    void addPlaceholderPage(
        NavigationController *navigation,
        Workspace *workspace,
        const QString &route,
        const QString &title
    )
    {
        auto *page = new QWidget(workspace);
        auto *titleLabel = new QLabel(page);
        auto *layout = new QVBoxLayout(page);

        layout->setContentsMargins(
            CreatorMetrics::SpacingXLarge,
            CreatorMetrics::SpacingXLarge,
            CreatorMetrics::SpacingXLarge,
            CreatorMetrics::SpacingXLarge
        );

        titleLabel->setAlignment(Qt::AlignCenter);

        QPalette palette = titleLabel->palette();

        palette.setColor(QPalette::WindowText, CreatorColors::TextPrimary);
        titleLabel->setPalette(palette);

        QFont titleFont = titleLabel->font();

        titleFont.setPointSize(24);
        titleFont.setBold(true);
        titleLabel->setFont(titleFont);
        titleLabel->setText(title);
        layout->addWidget(titleLabel);

        navigation->addPage(CurrentPage{route, page});
    }
}

AppShell::AppShell(QWidget *parent)
    : QWidget(parent),
      localization_(new LocalizationService(this)),
      sidebar_(new Sidebar(this)),
      workspace_(new Workspace(this)),
      navigation_(new NavigationController(workspace_, this))
{
    localization_->load(QStringLiteral(":/localization/localization.tsv"));

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    MenuComposer::build(sidebar_, localization_);

    layout->addWidget(sidebar_);
    layout->addWidget(workspace_, 1);

    addPlaceholderPage(
        navigation_,
        workspace_,
        QStringLiteral("home"),
        localization_->text(QStringLiteral("home"))
    );

    addPlaceholderPage(
        navigation_,
        workspace_,
        QStringLiteral("projects"),
        localization_->text(QStringLiteral("projects"))
    );

    addPlaceholderPage(
        navigation_,
        workspace_,
        QStringLiteral("planning"),
        localization_->text(QStringLiteral("planning"))
    );

    addPlaceholderPage(
        navigation_,
        workspace_,
        QStringLiteral("tasks"),
        localization_->text(QStringLiteral("tasks"))
    );

    addPlaceholderPage(
        navigation_,
        workspace_,
        QStringLiteral("library"),
        localization_->text(QStringLiteral("library"))
    );

    addPlaceholderPage(
        navigation_,
        workspace_,
        QStringLiteral("analytics"),
        localization_->text(QStringLiteral("analytics"))
    );

    addPlaceholderPage(
        navigation_,
        workspace_,
        QStringLiteral("settings"),
        localization_->text(QStringLiteral("settings"))
    );

    connect(
        sidebar_,
        &Sidebar::routeTriggered,
        navigation_,
        &NavigationController::navigateTo
    );

    connect(
        navigation_,
        &NavigationController::currentPageChanged,
        sidebar_,
        &Sidebar::setActiveRoute
    );

    navigation_->navigateTo(QStringLiteral("home"));
}