#include "ErrorState.h"

#include <QFont>
#include <QLabel>
#include <QPalette>
#include <QPushButton>
#include <QVBoxLayout>

#include "../style/Colors.h"
#include "../style/Metrics.h"

ErrorState::ErrorState(QWidget *parent)
    : QWidget(parent),
      titleLabel_(new QLabel(this)),
      descriptionLabel_(new QLabel(this)),
      retryButton_(new QPushButton(this))
{
    auto *layout = new QVBoxLayout(this);

    layout->setContentsMargins(
        CreatorMetrics::SpacingXLarge,
        CreatorMetrics::SpacingXLarge,
        CreatorMetrics::SpacingXLarge,
        CreatorMetrics::SpacingXLarge
    );

    layout->setSpacing(CreatorMetrics::SpacingMedium);
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
    retryButton_->setVisible(false);
    layout->addWidget(titleLabel_);
    layout->addWidget(descriptionLabel_);
    layout->addWidget(retryButton_, 0, Qt::AlignCenter);

    connect(retryButton_, &QPushButton::clicked, this, &ErrorState::retryRequested);
}

void ErrorState::setTitle(const QString &title)
{
    titleLabel_->setText(title);
}

void ErrorState::setDescription(const QString &description)
{
    descriptionLabel_->setText(description);
    descriptionLabel_->setVisible(!description.isEmpty());
}

void ErrorState::setRetryText(const QString &text)
{
    retryButton_->setText(text);
    retryButton_->setVisible(!text.isEmpty());
}