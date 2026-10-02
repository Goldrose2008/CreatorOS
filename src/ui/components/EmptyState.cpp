#include "EmptyState.h"

#include <QFont>
#include <QLabel>
#include <QPalette>
#include <QVBoxLayout>

#include "../style/Colors.h"
#include "../style/Metrics.h"

EmptyState::EmptyState(QWidget *parent)
    : QWidget(parent),
      titleLabel_(new QLabel(this)),
      descriptionLabel_(new QLabel(this))
{
    auto *layout = new QVBoxLayout(this);

    layout->setContentsMargins(
        CreatorMetrics::SpacingXLarge,
        CreatorMetrics::SpacingXLarge,
        CreatorMetrics::SpacingXLarge,
        CreatorMetrics::SpacingXLarge
    );
    layout->setSpacing(CreatorMetrics::SpacingSmall);
    layout->setAlignment(Qt::AlignCenter);

    QFont titleFont = titleLabel_->font();

    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel_->setFont(titleFont);

    QPalette titlePalette = titleLabel_->palette();

    titlePalette.setColor(QPalette::WindowText, CreatorColors::TextPrimary);
    titleLabel_->setPalette(titlePalette);

    QPalette descriptionPalette = descriptionLabel_->palette();

    descriptionPalette.setColor(QPalette::WindowText, CreatorColors::TextSecondary);
    descriptionLabel_->setPalette(descriptionPalette);
    descriptionLabel_->setAlignment(Qt::AlignCenter);
    descriptionLabel_->setWordWrap(true);
    layout->addWidget(titleLabel_);
    layout->addWidget(descriptionLabel_);

    setLayout(layout);
}

void EmptyState::setTitle(const QString &title)
{
    titleLabel_->setText(title);
}

void EmptyState::setDescription(const QString &description)
{
    descriptionLabel_->setText(description);
    descriptionLabel_->setVisible(!description.isEmpty());
}