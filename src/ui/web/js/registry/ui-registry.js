export const uiRegistry = {

    "app.title": {
        type: "text",
        textKey: "app.title"
    },

    "menu.section.workspace": {
        type: "menu-section",
        textKey: "menu.section.workspace"
    },

    "menu.item.home": {
        type: "menu-item",
        textKey: "menu.item.home",
        icon: "home",
        route: "home",
        active: true
    },

    "menu.item.projects": {
        type: "menu-item",
        textKey: "menu.item.projects",
        icon: "projects",
        route: "projects"
    },

    "menu.item.planning": {
        type: "menu-item",
        textKey: "menu.item.planning",
        icon: "planning",
        route: "planning"
    },

    "menu.item.tasks": {
        type: "menu-item",
        textKey: "menu.item.tasks",
        icon: "tasks",
        route: "tasks"
    },

    "menu.item.library": {
        type: "menu-item",
        textKey: "menu.item.library",
        icon: "library",
        route: "library"
    },

    "menu.item.analytics": {
        type: "menu-item",
        textKey: "menu.item.analytics",
        icon: "analytics",
        route: "analytics"
    },

    "menu.section.system": {
        type: "menu-section",
        textKey: "menu.section.system"
    },

    "menu.item.settings": {
        type: "menu-item",
        textKey: "menu.item.settings",
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