#include "EntityDetails.h"

#include <QVBoxLayout>
#include <QWidget>

#include "../../style/Metrics.h"

namespace
{
    void replaceWidget(
        QVBoxLayout *layout,
        QWidget *&currentWidget,
        QWidget *newWidget,
        QWidget *owner
    )
    {
        if (currentWidget == newWidget){return;}
        if (currentWidget != nullptr)
        {
            layout->removeWidget(currentWidget);
            currentWidget->deleteLater();
            currentWidget = nullptr;
        }
        if (newWidget == nullptr){return;}

        newWidget->setParent(owner);
        layout->addWidget(newWidget);
        currentWidget = newWidget;
    }
}

EntityDetails::EntityDetails(QWidget *parent)
    : QScrollArea(parent),
      contentWidget_(new QWidget(this)),
      layout_(new QVBoxLayout(contentWidget_)),
      navigationLayout_(new QVBoxLayout()),
      headerLayout_(new QVBoxLayout()),
      errorLayout_(new QVBoxLayout()),
      summaryLayout_(new QVBoxLayout()),
      contentLayout_(new QVBoxLayout()),
      navigationWidget_(nullptr),
      headerWidget_(nullptr),
      errorWidget_(nullptr),
      summaryWidget_(nullptr)
{
    setWidget(contentWidget_);
    setWidgetResizable(true);
    setFrameShape(QFrame::NoFrame);

    layout_->setContentsMargins(
        CreatorMetrics::SpacingXLarge,
        CreatorMetrics::SpacingXLarge,
        CreatorMetrics::SpacingXLarge,
        CreatorMetrics::SpacingXLarge
    );

    layout_->setSpacing(CreatorMetrics::SpacingLarge);

    navigationLayout_->setContentsMargins(0, 0, 0, 0);
    headerLayout_->setContentsMargins(0, 0, 0, 0);
    errorLayout_->setContentsMargins(0, 0, 0, 0);
    summaryLayout_->setContentsMargins(0, 0, 0, 0);
    contentLayout_->setContentsMargins(0, 0, 0, 0);
    contentLayout_->setSpacing(CreatorMetrics::SpacingLarge);

    layout_->addLayout(navigationLayout_);
    layout_->addLayout(headerLayout_);
    layout_->addLayout(errorLayout_);
    layout_->addLayout(summaryLayout_);
    layout_->addLayout(contentLayout_);
    layout_->addStretch();
}

void EntityDetails::setNavigationWidget(QWidget *widget)
{
    replaceWidget(navigationLayout_, navigationWidget_, widget, contentWidget_);
}

void EntityDetails::setHeaderWidget(QWidget *widget)
{
    replaceWidget(headerLayout_, headerWidget_, widget, contentWidget_);
}

void EntityDetails::setErrorWidget(QWidget *widget)
{
    replaceWidget(errorLayout_, errorWidget_, widget, contentWidget_);
}

void EntityDetails::setSummaryWidget(QWidget *widget)
{
    replaceWidget(summaryLayout_, summaryWidget_, widget, contentWidget_);
}

void EntityDetails::addContentWidget(QWidget *widget)
{
    if (widget == nullptr){return;}

    widget->setParent(contentWidget_);
    contentLayout_->addWidget(widget);
}

QVBoxLayout *EntityDetails::contentLayout() const
{
    return contentLayout_;
}