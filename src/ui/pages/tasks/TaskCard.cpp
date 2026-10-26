#include "TaskCard.h"

#include <QLabel>

#include "../../../ui/localization/LocalizationService.h"

TaskCard::TaskCard(const Task &task, LocalizationService &localization, QWidget *parent) : EntityCard(parent), task_(task)
{
    setTitle(QString::fromUtf8(task_.name.c_str()));
    setDescription(QString::fromUtf8(task_.description.c_str()));

    if (!task_.deadlineAt.empty())
    {
        auto *deadlineLabel = new QLabel(this);
        deadlineLabel->setText(localization.text(QStringLiteral("task.deadline")) + QStringLiteral(": ") + QString::fromUtf8(task_.deadlineAt.c_str()));
        addMetaWidget(deadlineLabel);
    }

    if (task_.parentTaskId.has_value())
    {
        auto *parentTaskLabel = new QLabel(this);
        parentTaskLabel->setText(localization.text(QStringLiteral("task.parent_task")) + QStringLiteral(": #") + QString::number(task_.parentTaskId.value()));
        addMetaWidget(parentTaskLabel);
    }

    setOpenAction(
        localization.text(QStringLiteral("common.action.open")),
        [this](){emit openRequested(task_.id);}
    );
}