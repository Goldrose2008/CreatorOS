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
      stateLayout_(new QVBoxLayout()),
      summaryLayout_(new QVBoxLayout()),
      contentLayout_(new QVBoxLayout()),
      navigationWidget_(nullptr),
      headerWidget_(nullptr),
      stateWidget_(nullptr),
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
    stateLayout_->setContentsMargins(0, 0, 0, 0);
    summaryLayout_->setContentsMargins(0, 0, 0, 0);
    contentLayout_->setContentsMargins(0, 0, 0, 0);
    contentLayout_->setSpacing(CreatorMetrics::SpacingLarge);

    layout_->addLayout(navigationLayout_);
    layout_->addLayout(headerLayout_);
    layout_->addLayout(stateLayout_);
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

void EntityDetails::setStateWidget(QWidget *widget)
{
    replaceWidget(stateLayout_, stateWidget_, widget, contentWidget_);
}

void EntityDetails::setSummaryWidget(QWidget *widget)
{
    replaceWidget(summaryLayout_, summaryWidget_, widget, contentWidget_);
}

void EntityDetails::setNavigationVisible(bool visible)
{
    if (navigationWidget_ != nullptr)
    {
        navigationWidget_->setVisible(visible);
    }
}

void EntityDetails::setHeaderVisible(bool visible)
{
    if (headerWidget_ != nullptr)
    {
        headerWidget_->setVisible(visible);
    }
}

void EntityDetails::setStateVisible(bool visible)
{
    if (stateWidget_ != nullptr)
    {
        stateWidget_->setVisible(visible);
    }
}

void EntityDetails::setSummaryVisible(bool visible)
{
    if (summaryWidget_ != nullptr)
    {
        summaryWidget_->setVisible(visible);
    }
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