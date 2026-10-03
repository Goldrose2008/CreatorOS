#pragma once

#include <QWidget>

#include <cstdint>

class EntityDetails;
class EntityHeader;
class QLabel;
class LocalizationService;
class ProjectService;
class Section;

class ProjectDetailsPage final : public QWidget
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

    EntityDetails *details_;
    EntityHeader *header_;

    QLabel *statusLabel_;
    QLabel *releaseLabel_;
    QLabel *progressLabel_;
    QLabel *ownerLabel_;
    QLabel *createdLabel_;
    QLabel *updatedLabel_;
    QLabel *descriptionLabel_;

    Section *summarySection_;
    Section *descriptionSection_;
};