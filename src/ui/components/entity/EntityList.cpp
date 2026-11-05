#include "EntityList.h"

#include <QFrame>
#include <QLayoutItem>
#include <QScrollArea>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "../states/EmptyState.h"
#include "../states/ErrorState.h"
#include "../states/LoadingState.h"
#include "../../localization/LocalizationService.h"
#include "../../style/Metrics.h"

EntityList::EntityList(LocalizationService &localization, QWidget *parent)
    : QWidget(parent),
      scrollArea_(new QScrollArea(this)),
      contentWidget_(new QWidget(scrollArea_)),
      layout_(new QVBoxLayout(contentWidget_)),
      stateStack_(new QStackedWidget(this)),
      loadingState_(new LoadingState(this)),
      emptyState_(new EmptyState(this)),
      errorState_(new ErrorState(this)),
      localization_(localization),
      itemCount_(0)
{
    scrollArea_->setWidget(contentWidget_);
    scrollArea_->setWidgetResizable(true);
    scrollArea_->setFrameShape(QFrame::NoFrame);

    layout_->setContentsMargins(0, 0, 0, 0);
    layout_->setSpacing(CreatorMetrics::SpacingMedium);
    layout_->addStretch();

    stateStack_->addWidget(scrollArea_);
    stateStack_->addWidget(loadingState_);
    stateStack_->addWidget(emptyState_);
    stateStack_->addWidget(errorState_);

    setLoadingTitle();
    setEmptyState();
    setErrorState();

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(stateStack_);

    connect(
        errorState_,
        &ErrorState::retryRequested,
        this,
        &EntityList::retryRequested
    );

    showList();
}

void EntityList::addItem(QWidget *item)
{
    if (item == nullptr)
        return;

    layout_->insertWidget(layout_->count() - 1, item);
    ++itemCount_;
}

void EntityList::clearItems()
{
    while (layout_->count() > 1)
    {
        QLayoutItem *layoutItem = layout_->takeAt(0);

        if (layoutItem == nullptr)
            continue;

        QWidget *widget = layoutItem->widget();
        delete layoutItem;

        if (widget != nullptr)
            widget->deleteLater();
    }

    itemCount_ = 0;
}

int EntityList::count() const
{
    return itemCount_;
}

void EntityList::setLoadingTitle(const QString &title)
{
    loadingState_->setTitle(title.isEmpty() ? localization_.text(QStringLiteral("common.state.loading")) : title);
}

void EntityList::setEmptyState(const QString &title, const QString &description)
{
    emptyState_->setTitle(title.isEmpty() ? localization_.text(QStringLiteral("common.state.empty.title")) : title);
    emptyState_->setDescription(description.isEmpty() ? localization_.text(QStringLiteral("common.state.empty.description")) : description);
}

void EntityList::setErrorState(const QString &title, const QString &description, const QString &retryText)
{
    errorState_->setTitle(title.isEmpty() ? localization_.text(QStringLiteral("common.state.error.title")) : title);
    errorState_->setDescription(description.isEmpty() ? localization_.text(QStringLiteral("common.state.error.load")) : description);
    errorState_->setRetryText(retryText.isEmpty() ? localization_.text(QStringLiteral("common.action.retry")) : retryText);
}

void EntityList::showLoading()
{
    stateStack_->setCurrentWidget(loadingState_);
}

void EntityList::showList()
{
    stateStack_->setCurrentWidget(scrollArea_);
}

void EntityList::showEmpty()
{
    stateStack_->setCurrentWidget(emptyState_);
}

void EntityList::showError()
{
    stateStack_->setCurrentWidget(errorState_);
}