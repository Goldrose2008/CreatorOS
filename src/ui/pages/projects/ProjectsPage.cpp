#include "ProjectsPage.h"

#include <QVBoxLayout>
#include <QStackedWidget>
#include <exception>

#include "../../../application/projects/ProjectService.h"
#include "../../../domain/models/Project.h"
#include "../../components/lists/EntityList.h"
#include "../../components/layout/PageHeader.h"
#include "../../components/states/EmptyState.h"
#include "../../components/states/ErrorState.h"
#include "../../components/states/LoadingState.h"
#include "../../style/Metrics.h"
#include "../../localization/LocalizationService.h"
#include "ProjectCard.h"

ProjectsPage::ProjectsPage(
    ProjectService &projectService,
    LocalizationService &localization,
    QWidget *parent)
    : QWidget(parent),
      projectService_(projectService),
      localization_(localization),
      header_(new PageHeader(this)),
      projectList_(new EntityList(this)),
      loadingState_(new LoadingState(this)),
      emptyState_(new EmptyState(this)),
      errorState_(new ErrorState(this)),
      stateStack_(new QStackedWidget(this))
{
    auto *layout = new QVBoxLayout(this);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(CreatorMetrics::SpacingXLarge);

    header_->setTitle(localization_.text(QStringLiteral("projects")));
    header_->setDescription(localization_.text(QStringLiteral("projects.description")));
    
    loadingState_->setTitle(localization_.text(QStringLiteral("projects.loading")));
    
    emptyState_->setTitle(localization_.text(QStringLiteral("projects.empty.title")));
    emptyState_->setDescription(localization_.text(QStringLiteral("projects.empty.description")));

    errorState_->setTitle(localization_.text(QStringLiteral("projects.error.title")));
    errorState_->setDescription(localization_.text(QStringLiteral("projects.error.description")));
    errorState_->setRetryText(localization_.text(QStringLiteral("projects.retry")));

    stateStack_->addWidget(projectList_);
    stateStack_->addWidget(loadingState_);
    stateStack_->addWidget(emptyState_);
    stateStack_->addWidget(errorState_);

    layout->addWidget(header_);
    layout->addWidget(stateStack_, 1);

    connect(
        errorState_,
        &ErrorState::retryRequested,
        this,
        &ProjectsPage::reload
    );

    reload();
}

void ProjectsPage::reload()
{
    stateStack_->setCurrentWidget(loadingState_);
    projectList_->clearItems();

    try
    {
        const std::vector<Project> projects = projectService_.getProjects();

        if (projects.empty())
        {
            stateStack_->setCurrentWidget(emptyState_);
            return;
        }

        for (const Project &project : projects)
        {
            auto *card = new ProjectCard(project, localization_, projectList_);

            connect(
                card,
                &ProjectCard::openRequested,
                this,
                &ProjectsPage::projectOpenRequested
            );

            projectList_->addItem(card);
        }

        stateStack_->setCurrentWidget(projectList_);
    }
    catch (const std::exception &)
    {
        stateStack_->setCurrentWidget(errorState_);
    }
}