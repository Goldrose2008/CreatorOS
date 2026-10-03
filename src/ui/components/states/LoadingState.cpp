#include "LoadingState.h"

#include <QLabel>
#include <QPalette>
#include <QVBoxLayout>

#include "../../style/Colors.h"
#include "../../style/Metrics.h"

LoadingState::LoadingState(QWidget *parent) : QWidget(parent), label_(new QLabel(this))
{
    auto *layout = new QVBoxLayout(this);

    layout->setContentsMargins(
        CreatorMetrics::SpacingXLarge,
        CreatorMetrics::SpacingXLarge,
        CreatorMetrics::SpacingXLarge,
        CreatorMetrics::SpacingXLarge
    );

    layout->setAlignment(Qt::AlignCenter);
    label_->setAlignment(Qt::AlignCenter);

    QPalette palette = label_->palette();

    palette.setColor(QPalette::WindowText, CreatorColors::TextSecondary);
    label_->setPalette(palette);
    layout->addWidget(label_);
}

void LoadingState::setText(const QString &text)
{
    label_->setText(text);
}