#include "MenuComposer.h"

#include "../components/navigation/MenuItem.h"
#include "../components/navigation/MenuSection.h"
#include "../localization/LocalizationService.h"
#include "../style/Icons.h"
#include "Sidebar.h"

namespace
{
    void addItem(
        Sidebar *sidebar,
        const LocalizationService *localization,
        const QString &textId,
        IconId icon,
        const QString &route
    )
    {
        auto *item = new MenuItem(sidebar);

        item->setIcon(Icons::get(icon));
        item->setText(localization->text(textId));
        item->setRoute(route);
        sidebar->addItem(item);
    }
}

void MenuComposer::build(
    Sidebar *sidebar,
    const LocalizationService *localization
)
{
    auto *workspace = new MenuSection(sidebar);

    workspace->setText(localization->text(QStringLiteral("common.entity.workspace")));

    sidebar->addSection(workspace);

    addItem(
        sidebar,
        localization,
        QStringLiteral("home"),
        IconId::Home,
        QStringLiteral("home")
    );

    addItem(
        sidebar,
        localization,
        QStringLiteral("projects"),
        IconId::Projects,
        QStringLiteral("projects")
    );

    addItem(
        sidebar,
        localization,
        QStringLiteral("planning"),
        IconId::Planning,
        QStringLiteral("planning")
    );

    addItem(
        sidebar,
        localization,
        QStringLiteral("tasks"),
        IconId::Tasks,
        QStringLiteral("tasks")
    );

    addItem(
        sidebar,
        localization,
        QStringLiteral("library"),
        IconId::Library,
        QStringLiteral("library")
    );

    addItem(
        sidebar,
        localization,
        QStringLiteral("analytics"),
        IconId::Analytics,
        QStringLiteral("analytics")
    );

    auto *system = new MenuSection(sidebar);

    system->setText(localization->text(QStringLiteral("system")));

    sidebar->addSection(system);

    addItem(
        sidebar,
        localization,
        QStringLiteral("settings"),
        IconId::Settings,
        QStringLiteral("settings")
    );
}