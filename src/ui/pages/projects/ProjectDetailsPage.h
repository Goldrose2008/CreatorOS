#pragma once

#include <cstdint>

#include "../../components/entity/EntityDetails.h"
#include "../../../domain/models/Project.h"

class EntityHeader;
class QLabel;
class LocalizationService;
class ProjectService;
class Section;
class QPushButton;

class ProjectDetailsPage final : public EntityDetails
{
    Q_OBJECT

public:
    explicit ProjectDetailsPage(ProjectService &projectService, LocalizationService &localization, QWidget *parent = nullptr);

public slots:
    void showProject(std::int64_t projectId);

signals:
    void backRequested();

private:
    ProjectService &projectService_;
    LocalizationService &localization_;

    EntityHeader *header_;
    QPushButton *editButton_;
    QPushButton *deleteButton_;

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
};