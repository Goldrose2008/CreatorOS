#include "ProjectsPage.h"

#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <exception>

#include "ProjectCard.h"
#include "ProjectEditorDialog.h"
#include "../../components/entity/EntityList.h"
#include "../../components/layout/PageHeader.h"
#include "../../localization/LocalizationService.h"
#include "../../style/Metrics.h"
#include "../../../application/projects/ProjectService.h"
#include "../../../domain/models/Project.h"

ProjectsPage::ProjectsPage(ProjectService &projectService, LocalizationService &localization, QWidget *parent)
    : QWidget(parent),
      projectService_(projectService),
      localization_(localization),
      header_(new PageHeader(this)),
      projectList_(new EntityList(this))
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(CreatorMetrics::SpacingXLarge);

    header_->setTitle(localization_.text(QStringLiteral("projects")));
    header_->setDescription(localization_.text(QStringLiteral("projects.description")));
    
    auto *createButton = new QPushButton(localization_.text(QStringLiteral("projects.create")), this);

    header_->addAction(createButton);

    connect(
        createButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            ProjectEditorDialog dialog(projectService_, localization_, this);

            if (dialog.exec() == QDialog::Accepted)
                reload();
        }
    );

    projectList_->setLoadingTitle(localization_.text(QStringLiteral("common.state.loading")));
    projectList_->setEmptyState(localization_.text(QStringLiteral("common.state.empty.title")), localization_.text(QStringLiteral("projects.empty.description")));
    projectList_->setErrorState(
        localization_.text(QStringLiteral("common.state.error.title")),
        localization_.text(QStringLiteral("common.state.error.load")),
        localization_.text(QStringLiteral("common.action.retry"))
    );

    layout->addWidget(header_);
    layout->addWidget(projectList_, 1);

    connect(
        projectList_,
        &EntityList::retryRequested,
        this,
        &ProjectsPage::reload
    );

    reload();
}

void ProjectsPage::reload()
{
    projectList_->showLoading();
    projectList_->clearItems();

    try
    {
        const std::vector<Project> projects = projectService_.getProjects();

        if (projects.empty())
        {
            projectList_->showEmpty();
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

        projectList_->showList();
    }
    catch (const std::exception &)
    {
        projectList_->showError();
    }
}