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
#include "../pages/content/ContentDetails.h"
#include "../pages/projects/ProjectsPage.h"
#include "../pages/projects/ProjectDetailsPage.h"
#include "../../application/projects/ProjectService.h"
#include "../../application/content/ContentService.h"

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

AppShell::AppShell(ProjectService &projectService, ContentService &contentService, QWidget *parent)
    : QWidget(parent),
      projectService_(&projectService),
      contentService_(&contentService),
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

    auto *projectsPage = new ProjectsPage(*projectService_, *localization_, workspace_);
    auto *projectDetailsPage = new ProjectDetailsPage(*projectService_, *contentService_, *localization_, workspace_);
    navigation_->addPage(CurrentPage{QStringLiteral("projects"), projectsPage});
    navigation_->addPage(CurrentPage{QStringLiteral("project-details"), projectDetailsPage});

    auto *contentDetails = new ContentDetails(*contentService_, *projectService_, *localization_, workspace_);
    navigation_->addPage(CurrentPage{QStringLiteral("content-details"), contentDetails});

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

    connect(
        projectsPage,
        &ProjectsPage::projectOpenRequested,
        this,
        [this, projectDetailsPage](std::int64_t projectId)
        {
            projectDetailsPage->showProject(projectId);
            navigation_->navigateTo(QStringLiteral("project-details"));
        }
    );

    connect(
        projectDetailsPage,
        &ProjectDetailsPage::contentOpenRequested,
        this,
        [this, contentDetails](std::int64_t contentId)
        {
            contentDetails->showContent(contentId);
            navigation_->navigateTo(QStringLiteral("content-details"));
        }
    );

    connect(
        projectDetailsPage,
        &ProjectDetailsPage::backRequested,
        this,
        [this, projectsPage]()
        {
            projectsPage->reload();
            navigation_->navigateTo(QStringLiteral("projects"));
        }
    );

    connect(
        contentDetails,
        &ContentDetails::backRequested,
        this,
        [this, projectDetailsPage](std::int64_t projectId)
        {
            projectDetailsPage->showProject(projectId);
            navigation_->navigateTo(QStringLiteral("project-details"));
        }
    );

    navigation_->navigateTo(QStringLiteral("home"));
}