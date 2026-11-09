#include "ContentDetails.h"

#include <QLabel>
#include <QGridLayout>
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
        return localization.text(QStringLiteral("content.role.") + QString::fromStdString(contentRoleToString(role)));
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
      summaryGrid_(new QGridLayout()),
      descriptionLabel_(createLabel(this)),
      summarySection_(new Section(this)),
      descriptionSection_(new Section(this))
{
    auto *backButton = new QPushButton(localization_.text(QStringLiteral("common.action.back")), this);

    connect(
        backButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            if (content_.id <= 0)
                return;

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
            if (content_.id <= 0)
                return;

            const std::int64_t contentId = content_.id;
            const std::int64_t projectId = content_.projectId;

            ConfirmModal::confirm(
                this,
                localization_.text(QStringLiteral("common.dialog.delete.title")),
                localization_.text(QStringLiteral("common.dialog.delete.description")),
                localization_.text(QStringLiteral("common.action.delete")),
                localization_.text(QStringLiteral("common.action.cancel")),
                [this, contentId, projectId]()
                {
                    try
                    {
                        if (!contentService_.deleteContent(contentId))
                        {
                            QMessageBox::critical(this, localization_.text(QStringLiteral("common.state.error.title")), localization_.text(QStringLiteral("content.delete.error.description")));
                            return;
                        }

                        content_ = {};
                        emit backRequested(projectId);
                    }
                    catch (const std::exception &)
                    {
                        QMessageBox::critical(this, localization_.text(QStringLiteral("common.state.error.title")), localization_.text(QStringLiteral("content.delete.error.description")));
                    }
                }
            );
        }
    );

    summarySection_->setTitle(localization_.text(QStringLiteral("common.label.summary")));

    summaryGrid_->setContentsMargins(0, 0, 0, 0);
    summarySection_->contentLayout()->addLayout(summaryGrid_);

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
        const auto content = contentService_.getContentByID(contentId);

        if (!content.has_value())
        {
            summarySection_->setVisible(false);
            descriptionSection_->setVisible(false);
            content_ = {};
            header_->setTitle(localization_.text(QStringLiteral("common.state.not_found.title")));
            header_->setDescription(localization_.text(QStringLiteral("common.state.not_found.description")));
            editButton_->setVisible(false);
            deleteButton_->setVisible(false);

            return;
        }

        const Content &value = content.value();
        const auto project = projectService_.getProjectByID(value.projectId);

        if (!project.has_value())
            throw std::runtime_error("Content project was not found.");

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
            throw std::runtime_error("Content type was not found.");

        content_ = value;
        header_->setTitle(QString::fromUtf8(value.name.c_str()));
        header_->setDescription(QString::fromUtf8(value.description.c_str()));
        
        const QList<EntityMetadata> metadata
        {
            {
                localization_.text(QStringLiteral("common.entity.project")),
                QString::fromUtf8(project->name.c_str())
            },
            {
                localization_.text(QStringLiteral("content.type")),
                contentTypeName
            },
            {
                localization_.text(QStringLiteral("content.role")),
                localizedContentRole(value.role, localization_)
            },
            {
                localization_.text(QStringLiteral("common.label.priority")),
                QString::number(value.priority)
            },
            {
                localization_.text(QStringLiteral("content.deadline")),
                value.productionDeadlineAt.empty() ? QStringLiteral("—") : QString::fromUtf8(value.productionDeadlineAt.c_str())
            },
            {
                localization_.text(QStringLiteral("common.label.progress")),
                QString::number(value.progress) + QStringLiteral("%")
            },
            {
                localization_.text(QStringLiteral("common.meta.created")),
                QString::fromUtf8(value.createdAt.c_str())
            },
            {
                localization_.text(QStringLiteral("common.meta.updated")),
                QString::fromUtf8(value.updatedAt.c_str())
            }
        };

    setMetadata(summaryGrid_, metadata);

        editButton_->setVisible(true);
        deleteButton_->setVisible(value.role == ContentRole::Additional);

        descriptionLabel_->setText(QString::fromUtf8(value.description.c_str()));
        summarySection_->setVisible(true);
        descriptionSection_->setVisible(!value.description.empty());
    }
    catch (const std::exception &)
    {
        summarySection_->setVisible(false);
        descriptionSection_->setVisible(false);
        content_ = {};

        header_->setTitle(localization_.text(QStringLiteral("common.state.error.title")));
        header_->setDescription(localization_.text(QStringLiteral("common.state.error.load")));
        
        editButton_->setVisible(false);
        deleteButton_->setVisible(false);    
    }
}