#include "ProjectCard.h"

#include "../../../ui/localization/LocalizationService.h"

ProjectCard::ProjectCard(const Project &project, LocalizationService &localization, QWidget *parent) : EntityCard(parent), project_(project)
{
    setTitle(QString::fromUtf8(project_.name.c_str()));
    setDescription(QString::fromUtf8(project_.description.c_str()));
    setProgress(project_.progress);
    setOpenAction(
        localization.text(QStringLiteral("common.action.open")),
        [this](){emit openRequested(project_.id);}
    );
}