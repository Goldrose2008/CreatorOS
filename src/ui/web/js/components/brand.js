function getInitial(name) {

    const normalized = name.trim();

    if (normalized.length === 0) {return "?";}

    return normalized.charAt(0)
        .toUpperCase();
}

export function buildBrandMark({
    id,
    name
}) {

    const element = document.createElement("div");

    element.id = id;
    element.className = "sidebar__brand-mark";
    element.textContent = getInitial(name);

    return element;
}

export function buildBrandName({
    id,
    name
}) {

    const element = document.createElement("div");

    element.id = id;
    element.className = "sidebar__brand-name";
    element.textContent = name;

    return element;
}