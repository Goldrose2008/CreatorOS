import { translate } from "../i18n/i18n.js";
import { getIcon } from "../icons.js";
import { buildMenuItem } from "../components/menu-item.js";
import { buildMenuSection } from "../components/menu-section.js";

const MAIN_MENU = [

    {
        slotId: "menu.section.01",
        type: "section",
        textId: "workspace"
    },

    {
        slotId: "menu.item.01",
        type: "item",
        iconId: "home",
        textId: "home",
        route: "home",
        active: true
    },

    {
        slotId: "menu.item.02",
        type: "item",
        iconId: "projects",
        textId: "projects",
        route: "projects"
    },

    {
        slotId: "menu.item.03",
        type: "item",
        iconId: "planning",
        textId: "planning",
        route: "planning"
    },

    {
        slotId: "menu.item.04",
        type: "item",
        iconId: "tasks",
        textId: "tasks",
        route: "tasks"
    },

    {
        slotId: "menu.item.05",
        type: "item",
        iconId: "library",
        textId: "library",
        route: "library"
    },

    {
        slotId: "menu.item.06",
        type: "item",
        iconId: "analytics",
        textId: "analytics",
        route: "analytics"
    },

    {
        slotId: "menu.section.02",
        type: "section",
        textId: "system"
    },

    {
        slotId: "menu.item.07",
        type: "item",
        iconId: "settings",
        textId: "settings",
        route: "settings"
    }

];

export function buildMainMenu() {
    MAIN_MENU.forEach(item => {
        const slot = document.getElementById(item.slotId);

        if (!slot) {
            console.warn(`Menu slot not found: ${item.slotId}`);
            return;
        }

        if (item.type === "section") {
            const section = buildMenuSection({
                    id: item.slotId,
                    text: translate(item.textId)
                });

            slot.replaceWith(section);
            return;
        }

        if (item.type === "item") {
            const menuItem = buildMenuItem({
                    id: item.slotId,
                    icon: getIcon(item.iconId),
                    text: translate(item.textId),
                    route: item.route,
                    active: item.active
                });

            slot.replaceWith(menuItem);
            return;
        }

        console.warn(`Unsupported menu item type: ${item.type}`);
    });
}