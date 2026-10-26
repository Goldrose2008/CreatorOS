#include "ContentCard.h"

#include <QLabel>

#include "../../../ui/localization/LocalizationService.h"

namespace
{
    QString roleText(ContentRole role, const LocalizationService &localization)
    {
        switch (role)
        {
        case ContentRole::Main:
            return localization.text(QStringLiteral("content.role.main"));
        case ContentRole::Additional:
            return localization.text(QStringLiteral("content.role.additional"));
        }

        return QString();
    }
}

ContentCard::ContentCard(
    const Content &content,
    const QString &contentTypeName,
    LocalizationService &localization,
    QWidget *parent
)
    : EntityCard(parent), content_(content)
{
    setTitle(QString::fromUtf8(content_.name.c_str()));
    setDescription(QString::fromUtf8(content_.description.c_str()));

    auto *roleLabel = new QLabel(this);
    roleLabel->setText(localization.text(QStringLiteral("content.role")) + QStringLiteral(": ") + roleText(content_.role, localization));
    addMetaWidget(roleLabel);

    if (!contentTypeName.isEmpty())
    {
        auto *typeLabel = new QLabel(this);
        typeLabel->setText(localization.text(QStringLiteral("content.type")) + QStringLiteral(": ") + contentTypeName);
        addMetaWidget(typeLabel);
    }

    auto *priorityLabel = new QLabel(this);
    priorityLabel->setText(localization.text(QStringLiteral("common.label.priority")) + QStringLiteral(": ") + QString::number(content_.priority));
    addMetaWidget(priorityLabel);

    if (!content_.productionDeadlineAt.empty())
    {
        auto *deadlineLabel = new QLabel(this);
        deadlineLabel->setText(localization.text(QStringLiteral("content.deadline")) + QStringLiteral(": ") + QString::fromUtf8(content_.productionDeadlineAt.c_str()));
        addMetaWidget(deadlineLabel);
    }

    setProgress(content_.progress);

    setOpenAction(
        localization.text(QStringLiteral("common.action.open")),
        [this](){emit openRequested(content_.id);}
    );
}