#include "ContentCard.h"

#include <QFont>
#include <QLabel>
#include <QPalette>

#include "../../../ui/localization/LocalizationService.h"
#include "../../../ui/style/Colors.h"

namespace
{
    QString roleText(
        ContentRole role,
        const LocalizationService &localization
    )
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

    QString statusText(
        ContentStatus status,
        const LocalizationService &localization
    )
    {
        switch (status)
        {
        case ContentStatus::Draft:
            return localization.text(QStringLiteral("content.status.draft"));
        case ContentStatus::InProgress:
            return localization.text(QStringLiteral("content.status.in_progress"));
        case ContentStatus::Ready:
            return localization.text(QStringLiteral("content.status.ready"));
        case ContentStatus::Archived:
            return localization.text(QStringLiteral("content.status.archived"));
        }

        return QString();
    }

    QLabel *createStatusLabel(
        const QString &text,
        QWidget *parent
    )
    {
        auto *label = new QLabel(parent);

        label->setText(text);
        label->setAutoFillBackground(true);

        QPalette palette = label->palette();
        palette.setColor(QPalette::Window, CreatorColors::AccentSoft);
        palette.setColor(QPalette::WindowText, CreatorColors::Accent);

        label->setPalette(palette);

        QFont font = label->font();
        font.setBold(true);
        label->setFont(font);

        return label;
    }
}

ContentCard::ContentCard(
    const Content &content,
    const QString &contentTypeName,
    LocalizationService &localization,
    QWidget *parent
)
    : EntityCard(parent),
      content_(content)
{
    setTitle(QString::fromUtf8(content_.name.c_str()));
    setDescription(QString::fromUtf8(content_.description.c_str()));

    auto *status = createStatusLabel(
        statusText(content_.status, localization),
        this
    );

    setStatusWidget(status);

    auto *roleLabel = new QLabel(this);

    roleLabel->setText(
        localization.text(QStringLiteral("content.role")) +
        QStringLiteral(": ") +
        roleText(content_.role, localization)
    );

    addMetaWidget(roleLabel);

    if (!contentTypeName.isEmpty())
    {
        auto *typeLabel = new QLabel(this);

        typeLabel->setText(
            localization.text(QStringLiteral("content.type")) +
            QStringLiteral(": ") +
            contentTypeName
        );

        addMetaWidget(typeLabel);
    }

    auto *priorityLabel = new QLabel(this);

    priorityLabel->setText(
        localization.text(QStringLiteral("content.priority")) +
        QStringLiteral(": ") +
        QString::number(content_.priority)
    );

    addMetaWidget(priorityLabel);

    if (!content_.productionDeadlineAt.empty())
    {
        auto *deadlineLabel = new QLabel(this);

        deadlineLabel->setText(
            localization.text(QStringLiteral("content.deadline")) +
            QStringLiteral(": ") +
            QString::fromUtf8(content_.productionDeadlineAt.c_str())
        );

        addMetaWidget(deadlineLabel);
    }

    setProgress(content_.progress);
}