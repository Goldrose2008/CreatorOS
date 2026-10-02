#include "EntityList.h"

#include <QVBoxLayout>
#include <QWidget>

#include "../style/Metrics.h"

EntityList::EntityList(QWidget *parent)
    : QScrollArea(parent),
      contentWidget_(new QWidget(this)),
      layout_(new QVBoxLayout(contentWidget_)),
      itemCount_(0)
{
    setWidget(contentWidget_);
    setWidgetResizable(true);
    setFrameShape(QFrame::NoFrame);

    layout_->setContentsMargins(0, 0, 0, 0);
    layout_->setSpacing(CreatorMetrics::SpacingMedium);
    layout_->addStretch();
}

void EntityList::addItem(QWidget *item)
{
    if (item == nullptr)
    {
        return;
    }

    layout_->insertWidget(layout_->count() - 1, item);
    ++itemCount_;
}

void EntityList::clearItems()
{
    while (layout_->count() > 1)
    {
        QLayoutItem *layoutItem = layout_->takeAt(0);

        if (layoutItem == nullptr)
        {
            continue;
        }

        QWidget *widget = layoutItem->widget();

        delete layoutItem;

        if (widget != nullptr)
        {
            widget->deleteLater();
        }
    }

    itemCount_ = 0;
}

int EntityList::count() const
{
    return itemCount_;
}