#include "NavigationController.h"

#include "../shell/Workspace.h"

NavigationController::NavigationController(
    Workspace *workspace, QObject *parent)
    : QObject(parent), workspace_(workspace)
{
}

bool NavigationController::addPage(const CurrentPage &page)
{
    if (page.route.trimmed().isEmpty()
        || page.widget == nullptr
        || pages_.contains(page.route))
    {
        return false;
    }

    workspace_->addWidget(page.widget);
    pages_.insert(page.route, page);

    return true;
}

bool NavigationController::navigateTo(const QString &route)
{
    const auto iterator = pages_.constFind(route);

    if (iterator == pages_.cend())
    {
        return false;
    }

    workspace_->setCurrentWidget(iterator.value().widget);

    if (currentRoute_ != route)
    {
        currentRoute_ = route;
        emit currentPageChanged(currentRoute_);
    }

    return true;
}

QString NavigationController::currentRoute() const
{
    return currentRoute_;
}