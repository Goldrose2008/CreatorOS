#include "ProjectDetailsPage.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "../../../application/projects/ProjectService.h"
#include "../../../domain/models/Project.h"
#include "../../components/entity/EntityDetails.h"
#include "../../components/entity/EntityHeader.h"
#include "../../components/layout/Section.h"
#include "../../localization/LocalizationService.h"
#include "../../style/Colors.h"
#include "../../style/Metrics.h"

namespace
{
    QString statusText(ProjectStatus status, const LocalizationService &localization)
    {
        switch (status)
        {
        case ProjectStatus::Draft:
            return localization.text(QStringLiteral("project.status.draft"));

        case ProjectStatus::Active:
            return localization.text(QStringLiteral("project.status.active"));

        case ProjectStatus::Archived:
            return localization.text(QStringLiteral("project.status.archived"));
        }

        return QString();
    }

    QLabel *createLabel(QWidget *parent)
    {
        auto *label = new QLabel(parent);
        QPalette palette = label->palette();
        palette.setColor(QPalette::WindowText, CreatorColors::TextPrimary);
        label->setPalette(palette);

        return label;
    }
}

ProjectDetailsPage::ProjectDetailsPage(
    ProjectService &projectService,
    LocalizationService &localization,
    QWidget *parent
)
    : QWidget(parent),
      projectService_(projectService),
      localization_(localization),
      details_(new EntityDetails(this)),
      header_(new EntityHeader(this)),
      statusLabel_(createLabel(this)),
      releaseLabel_(createLabel(this)),
      progressLabel_(createLabel(this)),
      ownerLabel_(createLabel(this)),
      createdLabel_(createLabel(this)),
      updatedLabel_(createLabel(this)),
      descriptionLabel_(createLabel(this)),
      summarySection_(new Section(this)),
      descriptionSection_(new Section(this))
{
    auto *layout = new QVBoxLayout(this);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(details_);

    auto *backButton = new QPushButton(localization_.text(QStringLiteral("project.back")), this);

    connect(
        backButton,
        &QPushButton::clicked,
        this,
        &ProjectDetailsPage::backRequested
    );

    details_->setNavigationWidget(backButton);
    details_->setHeaderWidget(header_);
    summarySection_->setTitle(localization_.text(QStringLiteral("project.summary")));

    auto *summaryLayout = summarySection_->contentLayout();
    auto *grid = new QGridLayout();

    grid->setContentsMargins(0, 0, 0, 0);
    grid->setHorizontalSpacing(CreatorMetrics::SpacingLarge);
    grid->setVerticalSpacing(CreatorMetrics::SpacingSmall);
    grid->addWidget(new QLabel(localization_.text(QStringLiteral("project.status")), summarySection_), 0, 0);
    grid->addWidget(statusLabel_, 0, 1);
    grid->addWidget(new QLabel(localization_.text(QStringLiteral("project.release")), summarySection_), 1, 0);
    grid->addWidget(releaseLabel_, 1, 1);
    grid->addWidget(new QLabel(localization_.text(QStringLiteral("project.progress")), summarySection_), 2, 0);
    grid->addWidget(progressLabel_, 2, 1);
    grid->addWidget(new QLabel(localization_.text(QStringLiteral("project.owner")), summarySection_), 3, 0);
    grid->addWidget(ownerLabel_, 3, 1);
    grid->addWidget(new QLabel(localization_.text(QStringLiteral("project.created")), summarySection_), 4, 0);
    grid->addWidget(createdLabel_, 4, 1);
    grid->addWidget(new QLabel(localization_.text(QStringLiteral("project.updated")), summarySection_), 5, 0);
    grid->addWidget(updatedLabel_, 5, 1);

    summaryLayout->addLayout(grid);

    descriptionSection_->setTitle(localization_.text(QStringLiteral("project.description")));
    descriptionSection_->contentLayout()->addWidget(descriptionLabel_);

    details_->setSummaryWidget(summarySection_);
    details_->addContentWidget(descriptionSection_);

    header_->setTitle(localization_.text(QStringLiteral("project.not_selected")));

    showProject(0);
}

void ProjectDetailsPage::showProject(std::int64_t projectId)
{
    if (projectId <= 0)
    {
        summarySection_->setVisible(false);
        descriptionSection_->setVisible(false);
        header_->setTitle(localization_.text(QStringLiteral("project.not_selected")));
        header_->setDescription(QString());

        return;
    }

    try
    {
        const auto project = projectService_.getProject(projectId);

        if (!project.has_value())
        {
            summarySection_->setVisible(false);
            descriptionSection_->setVisible(false);
            header_->setTitle(localization_.text(QStringLiteral("project.not_found.title")));
            header_->setDescription(localization_.text(QStringLiteral("project.not_found.description")));

            return;
        }

        const Project &value = project.value();

        summarySection_->setVisible(true);
        descriptionSection_->setVisible(!value.description.empty());
        header_->setTitle(QString::fromUtf8(value.name.c_str()));
        header_->setDescription(QString::fromUtf8(value.description.c_str()));

        auto *statusWidget = new QLabel(statusText(value.status, localization_), header_);

        header_->setStatusWidget(statusWidget);
        statusLabel_->setText(statusText(value.status, localization_));
        releaseLabel_->setText(QString::fromUtf8(value.plannedReleaseAt.c_str()));
        progressLabel_->setText(QString::number(value.progress) + QStringLiteral("%"));

        if (value.ownerId.has_value())
        {
            ownerLabel_->setText(QString::number(value.ownerId.value()));
        }
        else
        {
            ownerLabel_->setText(localization_.text(QStringLiteral("project.owner.none")));
        }

        createdLabel_->setText(QString::fromUtf8(value.createdAt.c_str()));
        updatedLabel_->setText(QString::fromUtf8(value.updatedAt.c_str()));
        descriptionLabel_->setText(QString::fromUtf8(value.description.c_str()));
    }
    catch (const std::exception &)
    {
        summarySection_->setVisible(false);
        descriptionSection_->setVisible(false);
        header_->setTitle(localization_.text(QStringLiteral("project.error.title")));
        header_->setDescription(localization_.text(QStringLiteral("project.error.description")));
    }
}