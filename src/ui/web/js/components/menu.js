const icons = {

    home: `
        <svg viewBox="0 0 24 24" aria-hidden="true">
            <path d="M3 10.5 12 3l9 7.5"></path>
            <path d="M5.5 9.5V21h13V9.5"></path>
            <path d="M9.5 21v-6h5v6"></path>
        </svg>
    `,

    projects: `
        <svg viewBox="0 0 24 24" aria-hidden="true">
            <rect x="3" y="4" width="18" height="16" rx="2"></rect>
            <path d="M7 8h10"></path>
            <path d="M7 12h7"></path>
            <path d="M7 16h5"></path>
        </svg>
    `,

    planning: `
        <svg viewBox="0 0 24 24" aria-hidden="true">
            <rect x="3" y="5" width="18" height="16" rx="2"></rect>
            <path d="M7 3v4"></path>
            <path d="M17 3v4"></path>
            <path d="M3 10h18"></path>
        </svg>
    `,

    tasks: `
        <svg viewBox="0 0 24 24" aria-hidden="true">
            <rect x="4" y="3" width="16" height="18" rx="2"></rect>
            <path d="m8 10 2 2 4-4"></path>
            <path d="M8 16h8"></path>
        </svg>
    `,

    library: `
        <svg viewBox="0 0 24 24" aria-hidden="true">
            <path d="M5 4h4v16H5z"></path>
            <path d="M10 4h4v16h-4z"></path>
            <path d="M15 4h4v16h-4z"></path>
        </svg>
    `,

    analytics: `
        <svg viewBox="0 0 24 24" aria-hidden="true">
            <path d="M4 19V5"></path>
            <path d="M4 19h16"></path>
            <path d="m7 15 4-4 3 2 5-6"></path>
        </svg>
    `,

    settings: `
        <svg viewBox="0 0 24 24" aria-hidden="true">
            <circle cx="12" cy="12" r="3"></circle>
            <path d="M19.4 15a1.7 1.7 0 0 0 .3 1.9l.1.1-1.8 1.8-.1-.1a1.7 1.7 0 0 0-1.9-.3 1.7 1.7 0 0 0-1 1.5V22h-2.6v-.1a1.7 1.7 0 0 0-1-1.5 1.7 1.7 0 0 0-1.9.3l-.1.1-1.8-1.8.1-.1a1.7 1.7 0 0 0 .3-1.9 1.7 1.7 0 0 0-1.5-1H5v-2.6h.1a1.7 1.7 0 0 0 1.5-1 1.7 1.7 0 0 0-.3-1.9l-.1-.1L8 6.6l.1.1a1.7 1.7 0 0 0 1.9.3 1.7 1.7 0 0 0 1-1.5V5h2.6v.1a1.7 1.7 0 0 0 1 1.5 1.7 1.7 0 0 0 1.9-.3l.1-.1 1.8 1.8-.1.1a1.7 1.7 0 0 0-.3 1.9 1.7 1.7 0 0 0 1.5 1h.1v2.6h-.1a1.7 1.7 0 0 0-1.5.9Z"></path>
        </svg>
    `
};

function getInitial(name) {
    const normalized = name.trim();
    return normalized.length > 0 ? normalized.charAt(0).toUpperCase() : "?";
}

export function buildMenuSection({
    id,
    label
}) {
    const element = document.createElement("div");

    element.className = "sidebar__section-title";
    element.dataset.uiId = id;

    element.textContent = label;

    return element;
}

export function buildMenuItem({
    id,
    label,
    icon,
    route,
    active = false
}) {
    const element = document.createElement("button");

    element.type = "button";
    element.className = "nav-item";

    if (active) {element.classList.add("active");}

    element.dataset.uiId = id;
    element.dataset.route = route;

    const iconElement = document.createElement("span");

    iconElement.className = "nav-item__icon";
    iconElement.setAttribute("aria-hidden", "true");
    iconElement.innerHTML = icons[icon] ?? "";

    const labelElement = document.createElement("span");

    labelElement.className = "nav-item__label";
    labelElement.textContent = label;

    element.append(iconElement, labelElement);

    return element;
}

export function buildBrandMark(name) {

    const element = document.createElement("div");

    element.className = "sidebar__brand-mark";
    element.textContent = getInitial(name);

    return element;
}

export function buildBrandName(name) {

    const element = document.createElement("div");

    element.className = "sidebar__brand-name";
    element.textContent = name;

    return element;
}