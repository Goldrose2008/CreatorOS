#include "ProjectCard.h"

#include <QLabel>

#include "../../../ui/localization/LocalizationService.h"

ProjectCard::ProjectCard(const Project &project, LocalizationService &localization, QWidget *parent) : EntityCard(parent), project_(project)
{
    setTitle(QString::fromUtf8(project_.name.c_str()));
    setDescription(QString::fromUtf8(project_.description.c_str()));

    auto *releaseDate = new QLabel(this);
    releaseDate->setText(localization.text(QStringLiteral("project.release")) + QStringLiteral(": ") + QString::fromUtf8(project_.plannedReleaseAt.c_str()));

    addMetaWidget(releaseDate);
    setProgress(project_.progress);

    setOpenAction(
        localization.text(QStringLiteral("common.action.open")),
        [this](){emit openRequested(project_.id);}
    );
}