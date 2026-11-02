#include "ContentCard.h"

#include "../../../ui/localization/LocalizationService.h"

ContentCard::ContentCard(const Content &content, LocalizationService &localization, QWidget *parent)
    : EntityCard(localization, parent), content_(content)
{
    setTitle(QString::fromUtf8(content_.name.c_str()));
    setDescription(QString::fromUtf8(content_.description.c_str()));
    setProgress(content_.progress);
    setOpenAction([this](){emit openRequested(content_.id);});
}