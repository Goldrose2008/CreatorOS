#include "TaskCard.h"

#include <QFont>
#include <QLabel>
#include <QPalette>

#include "../../../ui/localization/LocalizationService.h"
#include "../../../ui/style/Colors.h"

namespace
{
    QString statusText(TaskStatus status, const LocalizationService &localization)
    {
        switch (status)
        {
        case TaskStatus::Todo:
            return localization.text(QStringLiteral("task.status.todo"));
        case TaskStatus::InProgress:
            return localization.text(QStringLiteral("task.status.in_progress"));
        case TaskStatus::Done:
            return localization.text(QStringLiteral("task.status.done"));
        case TaskStatus::Cancelled:
            return localization.text(QStringLiteral("task.status.cancelled"));
        }

        return QString();
    }
}

TaskCard::TaskCard(const Task &task, LocalizationService &localization, QWidget *parent)
    : EntityCard(parent), task_(task)
{
    setTitle(QString::fromUtf8(task_.name.c_str()));
    setDescription(QString::fromUtf8(task_.description.c_str()));

    auto *status = createStatusLabel(statusText(task_.status, localization));
    setStatusWidget(status);

    if (!task_.deadlineAt.empty())
    {
        auto *deadlineLabel = new QLabel(this);

        deadlineLabel->setText(
            localization.text(QStringLiteral("task.deadline")) +
            QStringLiteral(": ") + QString::fromUtf8(task_.deadlineAt.c_str())
        );

        addMetaWidget(deadlineLabel);
    }

    if (task_.parentTaskId.has_value())
    {
        auto *parentTaskLabel = new QLabel(this);

        parentTaskLabel->setText(
            localization.text(QStringLiteral("task.parent_task")) +
            QStringLiteral(": #") + QString::number(task_.parentTaskId.value())
        );

        addMetaWidget(parentTaskLabel);
    }
}