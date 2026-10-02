export function buildMenuItem({
    id,
    icon,
    text,
    route,
    active = false
}) {

    const element = document.createElement("button");

    element.type = "button";
    element.id = id;
    element.className = "nav-item";

    if (active) {element.classList.add("active");}

    element.dataset.route = route;

    const iconElement = document.createElement("span");

    iconElement.className = "nav-item__icon";
    iconElement.setAttribute("aria-hidden", "true");
    iconElement.innerHTML = icon;

    const textElement = document.createElement("span");

    textElement.className = "nav-item__label";
    textElement.textContent = text;

    element.append(iconElement, textElement);

    return element;
}