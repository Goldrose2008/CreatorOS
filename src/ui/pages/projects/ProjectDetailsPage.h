#pragma once

#include <cstdint>

#include "../../components/entity/EntityDetails.h"
#include "../../../domain/models/Project.h"

class QLabel;
class QPushButton;
class QVBoxLayout;
class Section;

class EntityHeader;
class LocalizationService;
class ProjectService;
class ContentService;

class ProjectDetailsPage final : public EntityDetails
{
    Q_OBJECT

public:
    explicit ProjectDetailsPage(ProjectService &projectService, ContentService &contentService, LocalizationService &localization, QWidget *parent = nullptr);

public slots:
    void showProject(std::int64_t projectId);

signals:
    void backRequested();
    void contentOpenRequested(std::int64_t contentId);

private:
    ProjectService &projectService_;
    ContentService &contentService_;
    LocalizationService &localization_;

    EntityHeader *header_;
    QPushButton *editButton_;
    QPushButton *deleteButton_;
    QPushButton *addContentButton_;

    QLabel *statusLabel_;
    QLabel *releaseLabel_;
    QLabel *progressLabel_;
    QLabel *ownerLabel_;
    QLabel *createdLabel_;
    QLabel *updatedLabel_;
    QLabel *descriptionLabel_;
    
    Project project_;

    Section *summarySection_;
    Section *descriptionSection_;
    Section *contentSection_;
    
    QVBoxLayout *contentCardsLayout_;

    void clearContentCards();
};