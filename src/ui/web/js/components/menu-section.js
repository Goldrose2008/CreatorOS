export function buildMenuSection({
    id,
    text
}) {

    const element = document.createElement("div");

    element.id = id;
    element.className = "sidebar__section-title";
    element.textContent = text;

    return element;
}