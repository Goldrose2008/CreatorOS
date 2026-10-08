#include "ProjectCard.h"

#include <QFont>
#include <QLabel>
#include <QPalette>
#include <QPushButton>

#include "../../../ui/localization/LocalizationService.h"
#include "../../../ui/style/Colors.h"

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
}

ProjectCard::ProjectCard(const Project &project, LocalizationService &localization, QWidget *parent)
    : EntityCard(parent), project_(project)
{
    setTitle(QString::fromUtf8(project_.name.c_str()));
    setDescription(QString::fromUtf8(project_.description.c_str()));

    auto *status = createStatusLabel(statusText(project_.status, localization));

    setStatusWidget(status);

    auto *releaseDate = new QLabel(this);
    releaseDate->setText(localization.text(QStringLiteral("project.release")) + QStringLiteral(": ") + QString::fromUtf8(project_.plannedReleaseAt.c_str()));

    addMetaWidget(releaseDate);
    setProgress(project_.progress);

    auto *openButton = new QPushButton(localization.text(QStringLiteral("project.open")), this);

    addAction(openButton);

    connect(
        openButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            emit openRequested(project_.id);
        }
    );
}