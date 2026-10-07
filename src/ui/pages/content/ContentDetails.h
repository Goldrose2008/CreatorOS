#pragma once

#include <cstdint>

#include "../../components/entity/EntityDetails.h"
#include "../../../domain/models/Content.h"

class QLabel;
class Section;
class EntityHeader;
class LocalizationService;
class ProjectService;
class ContentService;

class ContentDetails final : public EntityDetails
{
    Q_OBJECT

public:
    explicit ContentDetails(
        ContentService &contentService,
        ProjectService &projectService,
        LocalizationService &localization,
        QWidget *parent = nullptr
    );

public slots:
    void showContent(std::int64_t contentId);

signals:
    void backRequested(std::int64_t projectId);

private:
    ContentService &contentService_;
    ProjectService &projectService_;
    LocalizationService &localization_;

    EntityHeader *header_;

    QLabel *projectLabel_;
    QLabel *typeLabel_;
    QLabel *roleLabel_;
    QLabel *statusLabel_;
    QLabel *priorityLabel_;
    QLabel *deadlineLabel_;
    QLabel *progressLabel_;
    QLabel *createdLabel_;
    QLabel *updatedLabel_;
    QLabel *descriptionLabel_;

    Content content_;

    Section *summarySection_;
    Section *descriptionSection_;
};