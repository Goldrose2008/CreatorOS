#include "ProjectDetailsPage.h"

#include <QLabel>
#include <QDialog>
#include <QMessageBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QPushButton>

#include "ProjectEditorDialog.h"
#include "../content/ContentCard.h"
#include "../../components/entity/EntityHeader.h"
#include "../../components/forms/ConfirmModal.h"
#include "../../components/layout/Section.h"
#include "../../localization/LocalizationService.h"
#include "../../style/Colors.h"
#include "../../style/Metrics.h"
#include "../../../application/projects/ProjectService.h"
#include "../../../application/content/ContentService.h"
#include "../../../domain/models/Project.h"
#include "../../../domain/models/ContentType.h"

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
    ContentService &contentService,
    LocalizationService &localization,
    QWidget *parent
)
    : EntityDetails(parent),
      projectService_(projectService),
      contentService_(contentService),
      localization_(localization),
      header_(new EntityHeader(this)),
      editButton_(new QPushButton(localization.text(QStringLiteral("project.edit")), this)),
      deleteButton_(new QPushButton(localization.text(QStringLiteral("project.delete")), this)),
      statusLabel_(createLabel(this)),
      releaseLabel_(createLabel(this)),
      progressLabel_(createLabel(this)),
      ownerLabel_(createLabel(this)),
      createdLabel_(createLabel(this)),
      updatedLabel_(createLabel(this)),
      descriptionLabel_(createLabel(this)),
      summarySection_(new Section(this)),
      descriptionSection_(new Section(this)),
      contentSection_(new Section(this)),
      contentCardsLayout_(new QVBoxLayout())
{
    auto *backButton = new QPushButton(localization_.text(QStringLiteral("project.back")), this);

    connect(
        backButton,
        &QPushButton::clicked,
        this,
        &ProjectDetailsPage::backRequested
    );

    setNavigationWidget(backButton);
    setHeaderWidget(header_);

    header_->addAction(editButton_);
    header_->addAction(deleteButton_);
    editButton_->setVisible(false);
    deleteButton_->setVisible(false);

    connect(
        editButton_,
        &QPushButton::clicked,
        this,
        [this]()
        {
            if (project_.id <= 0){return;}

            const std::int64_t projectId = project_.id;

            ProjectEditorDialog dialog(
                projectService_,
                localization_,
                this,
                project_
            );

            if (dialog.exec() == QDialog::Accepted)
            {
                showProject(projectId);
            }
        }
    );

    connect(
        deleteButton_,
        &QPushButton::clicked,
        this,
        [this]()
        {
            if (project_.id <= 0) {return;}

            const std::int64_t projectId = project_.id;

            ConfirmModal::confirm(
                this,
                localization_.text(QStringLiteral("project.delete.title")),
                localization_.text(QStringLiteral("project.delete.description")),
                localization_.text(QStringLiteral("project.delete.confirm")),
                localization_.text(QStringLiteral("project.delete.cancel")),
                [this, projectId]()
                {
                    try
                    {
                        if (!projectService_.deleteProject(projectId))
                        {
                            QMessageBox::critical(
                                this,
                                localization_.text(QStringLiteral("project.delete.error.title")),
                                localization_.text(QStringLiteral("project.delete.error.description"))
                            );

                            return;
                        }

                        project_ = {};
                        backRequested();
                    }
                    catch (const std::exception &)
                    {
                        QMessageBox::critical(
                            this,
                            localization_.text(QStringLiteral("project.delete.error.title")),
                            localization_.text(QStringLiteral("project.delete.error.description"))
                        );
                    }
                }
            );
        }
    );

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
    
    contentSection_->setTitle(localization_.text(QStringLiteral("project.content")));
    contentCardsLayout_->setContentsMargins(0, 0, 0, 0);
    contentCardsLayout_->setSpacing(CreatorMetrics::SpacingMedium);
    contentCardsLayout_->addStretch();
    contentSection_->contentLayout()->addLayout(contentCardsLayout_);

    setSummaryWidget(summarySection_);
    addContentWidget(contentSection_);
    addContentWidget(descriptionSection_);

    header_->setTitle(localization_.text(QStringLiteral("project.not_selected")));

    showProject(0);
}

void ProjectDetailsPage::clearContentCards()
{
    while (contentCardsLayout_->count() > 1)
    {
        QLayoutItem *layoutItem = contentCardsLayout_->takeAt(0);

        if (layoutItem == nullptr){continue;}

        QWidget *widget = layoutItem->widget();
        delete layoutItem;

        if (widget != nullptr){widget->deleteLater();}
    }
}

void ProjectDetailsPage::showProject(std::int64_t projectId)
{
    if (projectId <= 0)
    {
        summarySection_->setVisible(false);
        contentSection_->setVisible(false);
        descriptionSection_->setVisible(false);
        clearContentCards();
        header_->setTitle(localization_.text(QStringLiteral("project.not_selected")));
        header_->setDescription(QString());
        project_ = {};
        editButton_->setVisible(false);
        deleteButton_->setVisible(false);

        return;
    }

    try
    {
        const auto project = projectService_.getProject(projectId);

        if (!project.has_value())
        {
            summarySection_->setVisible(false);
            contentSection_->setVisible(false);
            descriptionSection_->setVisible(false);
            clearContentCards();
            header_->setTitle(localization_.text(QStringLiteral("project.not_found.title")));
            header_->setDescription(localization_.text(QStringLiteral("project.not_found.description")));
            project_ = {};
            editButton_->setVisible(false);
            deleteButton_->setVisible(false);

            return;
        }

        const Project &value = project.value();
        project_ = value;
        editButton_->setVisible(true);
        deleteButton_->setVisible(true);

        clearContentCards();

        const auto contents = contentService_.getProjectContents(value.id);
        const auto contentTypes = projectService_.getContentTypes();

        for (const Content &content : contents)
        {
            QString contentTypeName;

            for (const ContentType &contentType : contentTypes)
            {
                if (contentType.id == content.contentTypeId)
                {
                    contentTypeName = QString::fromUtf8(contentType.name.c_str());
                    break;
                }
            }

            auto *card = new ContentCard(
                content,
                contentTypeName,
                localization_,
                this
            );

            contentCardsLayout_->insertWidget(
                contentCardsLayout_->count() - 1,
                card
            );
        }

        contentSection_->setVisible(!contents.empty());
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
        contentSection_->setVisible(false);
        descriptionSection_->setVisible(false);
        clearContentCards();
        header_->setTitle(localization_.text(QStringLiteral("project.error.title")));
        header_->setDescription(localization_.text(QStringLiteral("project.error.description")));
        project_ = {};
        editButton_->setVisible(false);
        deleteButton_->setVisible(false);
    }
}