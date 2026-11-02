#include "TaskCard.h"

#include "../../../ui/localization/LocalizationService.h"

TaskCard::TaskCard(const Task &task, LocalizationService &localization, QWidget *parent) : EntityCard(parent), task_(task)
{
    setTitle(QString::fromUtf8(task_.name.c_str()));
    setDescription(QString::fromUtf8(task_.description.c_str()));
    setDeadline(
        localization.text(QStringLiteral("task.deadline")),
        QString::fromUtf8(task_.deadlineAt.c_str())
    );

    setOpenAction(
        localization.text(QStringLiteral("common.action.open")),
        [this](){emit openRequested(task_.id);}
    );
}