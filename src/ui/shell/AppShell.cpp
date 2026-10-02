#include "AppShell.h"

#include <QHBoxLayout>

#include "../localization/LocalizationService.h"
#include "MenuComposer.h"
#include "Sidebar.h"
#include "Workspace.h"

AppShell::AppShell(QWidget *parent)
    : QWidget(parent),
      localization_(new LocalizationService(this)),
      sidebar_(new Sidebar(this)),
      workspace_(new Workspace(this))
{
    localization_->load(QStringLiteral(":/localization/localization.tsv"));

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    MenuComposer::build(sidebar_, localization_);

    layout->addWidget(sidebar_);
    layout->addWidget(workspace_, 1);

    connect(
        sidebar_,
        &Sidebar::routeTriggered,
        this,
        [this](const QString &, const QString &text)
        {
            workspace_->setPageTitle(text);
        }
    );

    workspace_->setPageTitle(localization_->text(QStringLiteral("home")));
}