#include "Toolbar.h"

#include <QHBoxLayout>

#include "../../style/Metrics.h"

Toolbar::Toolbar(QWidget *parent) : QWidget(parent), layout_(new QHBoxLayout(this))
{
    layout_->setContentsMargins(0, 0, 0, 0);
    layout_->setSpacing(CreatorMetrics::SpacingSmall);
}

void Toolbar::addWidget(QWidget *widget)
{
    if (widget == nullptr)
    {
        return;
    }

    layout_->addWidget(widget);
}

void Toolbar::addStretch()
{
    layout_->addStretch();
}