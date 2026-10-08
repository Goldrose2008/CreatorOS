#include "ContentDetails.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDialog>
#include <QMessageBox>
#include <exception>

#include "ContentEditorDialog.h"
#include "../../components/entity/EntityHeader.h"
#include "../../components/forms/ConfirmModal.h"
#include "../../components/layout/Section.h"
#include "../../localization/LocalizationService.h"
#include "../../../application/content/ContentService.h"
#include "../../../application/projects/ProjectService.h"
#include "../../../domain/models/ContentType.h"

namespace
{
    QLabel *createLabel(QWidget *parent)
    {
        auto *label = new QLabel(parent);

        label->setWordWrap(true);

        return label;
    }

    QString localizedContentRole(ContentRole role, const LocalizationService &localization)
    {
        return localization.text(
            QStringLiteral("content.role.") +
            QString::fromStdString(contentRoleToString(role))
        );
    }

    QString localizedContentStatus(ContentStatus status, const LocalizationService &localization)
    {
        return localization.text(
            QStringLiteral("common.status.") +
            QString::fromStdString(contentStatusToString(status))
        );
    }
}

ContentDetails::ContentDetails(
    ContentService &contentService,
    ProjectService &projectService,
    LocalizationService &localization,
    QWidget *parent
)
    : EntityDetails(parent),
      contentService_(contentService),
      projectService_(projectService),
      localization_(localization),
      header_(new EntityHeader(this)),
      editButton_(new QPushButton(localization.text(QStringLiteral("common.action.edit")), this)),
      deleteButton_(new QPushButton(localization.text(QStringLiteral("common.action.delete")), this)),
      projectLabel_(createLabel(this)),
      typeLabel_(createLabel(this)),
      roleLabel_(createLabel(this)),
      statusLabel_(createLabel(this)),
      priorityLabel_(createLabel(this)),
      deadlineLabel_(createLabel(this)),
      progressLabel_(createLabel(this)),
      createdLabel_(createLabel(this)),
      updatedLabel_(createLabel(this)),
      descriptionLabel_(createLabel(this)),
      summarySection_(new Section(this)),
      descriptionSection_(new Section(this))
{
    auto *backButton = new QPushButton(localization_.text(QStringLiteral("content.back")), this);

    connect(
        backButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            if (content_.id <= 0)
            {
                return;
            }

            emit backRequested(content_.projectId);
        }
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
            if (content_.id <= 0){return;}

            const std::int64_t contentId = content_.id;

            ContentEditorDialog dialog(
                contentService_,
                projectService_,
                localization_,
                this,
                content_.projectId,
                content_
            );

            if (dialog.exec() == QDialog::Accepted)
            {
                showContent(contentId);
            }
        }
    );

    connect(
        deleteButton_,
        &QPushButton::clicked,
        this,
        [this]()
        {
            if (content_.id <= 0){return;}

            const std::int64_t contentId = content_.id;
            const std::int64_t projectId = content_.projectId;

            ConfirmModal::confirm(
                this,
                localization_.text(QStringLiteral("content.delete.title")),
                localization_.text(QStringLiteral("content.delete.description")),
                localization_.text(QStringLiteral("common.action.delete")),
                localization_.text(QStringLiteral("common.action.cancel")),
                [this, contentId, projectId]()
                {
                    try
                    {
                        if (!contentService_.deleteContent(contentId))
                        {
                            QMessageBox::critical(
                                this,
                                localization_.text(QStringLiteral("content.delete.error.title")),
                                localization_.text(QStringLiteral("content.delete.error.description"))
                            );

                            return;
                        }

                        content_ = {};
                        emit backRequested(projectId);
                    }
                    catch (const std::exception &)
                    {
                        QMessageBox::critical(
                            this,
                            localization_.text(QStringLiteral("content.delete.error.title")),
                            localization_.text(QStringLiteral("content.delete.error.description"))
                        );
                    }
                }
            );
        }
    );

    summarySection_->setTitle(localization_.text(QStringLiteral("common.label.summary")));

    auto *summaryLayout = summarySection_->contentLayout();
    auto *grid = new QGridLayout();

    grid->setContentsMargins(0, 0, 0, 0);
    grid->addWidget(new QLabel(localization_.text(QStringLiteral("common.entity.project")), summarySection_), 0, 0);
    grid->addWidget(projectLabel_, 0, 1);
    grid->addWidget(new QLabel(localization_.text(QStringLiteral("content.type")), summarySection_), 1, 0);
    grid->addWidget(typeLabel_, 1, 1);
    grid->addWidget(new QLabel(localization_.text(QStringLiteral("content.role")), summarySection_), 2, 0);
    grid->addWidget(roleLabel_, 2, 1);
    grid->addWidget(new QLabel(localization_.text(QStringLiteral("content.status")), summarySection_), 3, 0);
    grid->addWidget(statusLabel_, 3, 1);
    grid->addWidget(new QLabel(localization_.text(QStringLiteral("common.label.priority")), summarySection_), 4, 0);
    grid->addWidget(priorityLabel_, 4, 1);
    grid->addWidget(new QLabel(localization_.text(QStringLiteral("content.deadline")), summarySection_), 5, 0);
    grid->addWidget(deadlineLabel_, 5, 1);
    grid->addWidget(new QLabel(localization_.text(QStringLiteral("content.progress")), summarySection_), 6, 0);
    grid->addWidget(progressLabel_, 6, 1);
    grid->addWidget(new QLabel(localization_.text(QStringLiteral("common.meta.created")), summarySection_), 7, 0);
    grid->addWidget(createdLabel_, 7, 1);
    grid->addWidget(new QLabel(localization_.text(QStringLiteral("common.meta.updated")), summarySection_), 8, 0);
    grid->addWidget(updatedLabel_, 8, 1);
    summaryLayout->addLayout(grid);

    descriptionSection_->setTitle(localization_.text(QStringLiteral("common.label.description")));
    descriptionSection_->contentLayout()->addWidget(descriptionLabel_);

    setSummaryWidget(summarySection_);
    addContentWidget(descriptionSection_);

    header_->setTitle(localization_.text(QStringLiteral("common.entity.content")));

    showContent(0);
}

void ContentDetails::showContent(std::int64_t contentId)
{
    if (contentId <= 0)
    {
        summarySection_->setVisible(false);
        descriptionSection_->setVisible(false);
        content_ = {};
        header_->setTitle(localization_.text(QStringLiteral("common.entity.content")));
        header_->setDescription(QString());
        editButton_->setVisible(false);
        deleteButton_->setVisible(false);

        return;
    }

    try
    {
        const auto content = contentService_.getContent(contentId);

        if (!content.has_value())
        {
            summarySection_->setVisible(false);
            descriptionSection_->setVisible(false);
            content_ = {};
            header_->setTitle(localization_.text(QStringLiteral("content.not_found.title")));
            header_->setDescription(localization_.text(QStringLiteral("content.not_found.description")));
            editButton_->setVisible(false);
            deleteButton_->setVisible(false);

            return;
        }

        const Content &value = content.value();

        const auto project = projectService_.getProject(value.projectId);

        if (!project.has_value())
        {
            throw std::runtime_error("Content project was not found.");
        }

        const auto contentTypes = projectService_.getContentTypes();

        QString contentTypeName;

        for (const ContentType &contentType : contentTypes)
        {
            if (contentType.id == value.contentTypeId)
            {
                contentTypeName = QString::fromUtf8(contentType.name.c_str());
                break;
            }
        }

        if (contentTypeName.isEmpty())
        {
            throw std::runtime_error("Content type was not found.");
        }

        content_ = value;
        header_->setTitle(QString::fromUtf8(value.name.c_str()));
        header_->setDescription(QString::fromUtf8(value.description.c_str()));
        projectLabel_->setText(QString::fromUtf8(project->name.c_str()));
        typeLabel_->setText(contentTypeName);
        roleLabel_->setText(localizedContentRole(value.role, localization_));
        statusLabel_->setText(localizedContentStatus(value.status, localization_));
        priorityLabel_->setText(QString::number(value.priority));
        editButton_->setVisible(true);
        deleteButton_->setVisible(value.role == ContentRole::Additional);

        if (value.productionDeadlineAt.empty())
        {
            deadlineLabel_->setText(QStringLiteral("—"));
        }
        else
        {
            deadlineLabel_->setText(QString::fromUtf8(value.productionDeadlineAt.c_str()));
        }

        progressLabel_->setText(QString::number(value.progress) + QStringLiteral("%"));
        createdLabel_->setText(QString::fromUtf8(value.createdAt.c_str()));
        updatedLabel_->setText(QString::fromUtf8(value.updatedAt.c_str()));
        descriptionLabel_->setText(QString::fromUtf8(value.description.c_str()));
        summarySection_->setVisible(true);
        descriptionSection_->setVisible(!value.description.empty());
    }
    catch (const std::exception &)
    {
        summarySection_->setVisible(false);
        descriptionSection_->setVisible(false);
        content_ = {};

        header_->setTitle(localization_.text(QStringLiteral("content.error.title")));
        header_->setDescription(localization_.text(QStringLiteral("content.error.description")));
        
        editButton_->setVisible(false);
        deleteButton_->setVisible(false);    
    }
}