#include "StateWidget.h"

#include <QFont>
#include <QLabel>
#include <QPalette>
#include <QVBoxLayout>

#include "../../style/Colors.h"
#include "../../style/Metrics.h"

StateWidget::StateWidget(QWidget *parent)
    : QWidget(parent),
      titleLabel_(new QLabel(this)),
      descriptionLabel_(new QLabel(this)),
      layout_(new QVBoxLayout(this))
{
    layout_->setContentsMargins(
        CreatorMetrics::SpacingXLarge,
        CreatorMetrics::SpacingXLarge,
        CreatorMetrics::SpacingXLarge,
        CreatorMetrics::SpacingXLarge
    );

    layout_->setSpacing(CreatorMetrics::SpacingSmall);
    layout_->setAlignment(Qt::AlignCenter);

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
    descriptionLabel_->setVisible(false);

    layout_->addWidget(titleLabel_);
    layout_->addWidget(descriptionLabel_);
}

void StateWidget::setTitle(const QString &title)
{
    titleLabel_->setText(title);
}

void StateWidget::setDescription(const QString &description)
{
    descriptionLabel_->setText(description);

    descriptionLabel_->setVisible(!description.isEmpty());
}

QVBoxLayout *StateWidget::contentLayout() const
{
    return layout_;
}