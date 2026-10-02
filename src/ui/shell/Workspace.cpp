#include "Workspace.h"

#include <QLabel>
#include <QPalette>
#include <QVBoxLayout>

#include "../style/Colors.h"
#include "../style/Metrics.h"

Workspace::Workspace(QWidget *parent) : QStackedWidget(parent)
{
    auto *page = new QWidget(this);

    placeholderTitle_ = new QLabel(page);

    auto *layout = new QVBoxLayout(page);

    layout->setContentsMargins(
        CreatorMetrics::SpacingXLarge,
        CreatorMetrics::SpacingXLarge,
        CreatorMetrics::SpacingXLarge,
        CreatorMetrics::SpacingXLarge
    );

    placeholderTitle_->setAlignment(Qt::AlignCenter);

    QPalette palette = placeholderTitle_->palette();

    palette.setColor(QPalette::WindowText, CreatorColors::TextPrimary);

    placeholderTitle_->setPalette(palette);

    QFont titleFont = placeholderTitle_->font();

    titleFont.setPointSize(24);
    titleFont.setBold(true);

    placeholderTitle_->setFont(titleFont);

    layout->addWidget(placeholderTitle_);

    addWidget(page);
    setCurrentWidget(page);
}

void Workspace::setPageTitle(const QString &title)
{
    placeholderTitle_->setText(title);
}