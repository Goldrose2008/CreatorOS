export const uiRegistry = {

    "app.brand.mark": {type: "brand-mark"},
    "app.brand.name": {type: "app-name"},
    "app.version": {type: "app-version"},

    "menu.navigation.label": {
        type: "attribute",
        attribute: "aria-label",
        textKey: "menu.navigation.label"
    },

    "menu.section.01": {
        type: "menu-section",
        textKey: "menu.section.01"
    },
    "menu.section.02": {
        type: "menu-section",
        textKey: "menu.section.02"
    },

    "menu.item.01": {
        type: "menu-item",
        textKey: "menu.item.01",
        icon: "home",
        route: "home",
        active: true
    },
    "menu.item.02": {
        type: "menu-item",
        textKey: "menu.item.02",
        icon: "projects",
        route: "projects"
    },
    "menu.item.03": {
        type: "menu-item",
        textKey: "menu.item.03",
        icon: "planning",
        route: "planning"
    },
    "menu.item.04": {
        type: "menu-item",
        textKey: "menu.item.04",
        icon: "tasks",
        route: "tasks"
    },
    "menu.item.05": {
        type: "menu-item",
        textKey: "menu.item.05",
        icon: "library",
        route: "library"
    },
    "menu.item.06": {
        type: "menu-item",
        textKey: "menu.item.06",
        icon: "analytics",
        route: "analytics"
    },
    "menu.item.07": {
        type: "menu-item",
        textKey: "menu.item.07",
        icon: "settings",
        route: "settings"
    },

    "page.home.title": {
        type: "text",
        textKey: "page.home.title"
    },
    "page.home.context": {
        type: "text",
        textKey: "page.home.context"
    },
    "page.home.eyebrow": {
        type: "text",
        textKey: "page.home.eyebrow"
    },
    "page.home.heading": {
        type: "text",
        textKey: "page.home.heading"
    },
    "page.home.description": {
        type: "text",
        textKey: "page.home.description"
    },

    "dashboard.projects.label": {
        type: "text",
        textKey: "dashboard.projects.label"
    },
    "dashboard.projects.hint": {
        type: "text",
        textKey: "dashboard.projects.hint"
    },

    "dashboard.tasks.label": {
        type: "text",
        textKey: "dashboard.tasks.label"
    },
    "dashboard.tasks.hint": {
        type: "text",
        textKey: "dashboard.tasks.hint"
    },

    "dashboard.publications.label": {
        type: "text",
        textKey: "dashboard.publications.label"
    },
    "dashboard.publications.hint": {
        type: "text",
        textKey: "dashboard.publications.hint"
    },

    "bridge.connecting": {
        type: "text",
        textKey: "bridge.connecting"
    },
    "bridge.connected": {
        type: "text",
        textKey: "bridge.connected"
    },
    "bridge.error": {
        type: "text",
        textKey: "bridge.error"
    }
};

export function getUiResource(id) {
    return uiRegistry[id] ?? null;
}