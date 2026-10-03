#include "ProjectCard.h"

#include <QFont>
#include <QLabel>
#include <QPalette>
#include <QPushButton>
#include <QVBoxLayout>

#include "../../../ui/components/entity/EntityCard.h"
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

    QLabel *createStatusLabel(const QString &text, QWidget *parent)
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

ProjectCard::ProjectCard(const Project &project, LocalizationService &localization, QWidget *parent)
    : QWidget(parent), project_(project), card_(new EntityCard(this))
{
    auto *layout = new QVBoxLayout(this);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(card_);

    card_->setTitle(QString::fromUtf8(project_.name.c_str()));
    card_->setDescription(QString::fromUtf8(project_.description.c_str()));

    auto *status = createStatusLabel(statusText(project_.status, localization), card_);

    card_->setStatusWidget(status);

    auto *releaseDate = new QLabel(card_);
    releaseDate->setText(localization.text(QStringLiteral("project.release")) + QStringLiteral(": ") + QString::fromUtf8(project_.plannedReleaseAt.c_str()));

    card_->addMetaWidget(releaseDate);
    card_->setProgress(project_.progress);

    auto *openButton = new QPushButton(localization.text(QStringLiteral("project.open")), card_);

    card_->addAction(openButton);

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