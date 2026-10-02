import { connectBridge } from "./bridge.js";
import { getUiResource } from "./registry/ui-registry.js";
import {
    getLocale,
    translate
} from "./i18n/i18n.js";
import {
    buildMenuItem,
    buildMenuSection,
    buildBrandMark,
    buildBrandName
} from "./components/menu.js";

const connectionDot = document.getElementById("connection-dot");
const appVersion = document.getElementById("app-version");

function renderUiResource(element) {
    const id = element.dataset.uiId;
    const resource = getUiResource(id);
    
    if (!resource) {
        console.warn(`UI resource not found: ${id}`);
        return;
    } 

    const label = translate(resource.textKey);

    if (resource.type === "text") {
        element.textContent = label;
        return;
    }

    if (resource.type === "brand-mark") {
        const brandMark = buildBrandMark(applicationName);
        element.replaceWith(brandMark);
        return;
    }

    if (resource.type === "app-name") {
        const brandName = buildBrandName(applicationName);
        element.replaceWith(brandName);
        return;
    }

    if (resource.type === "menu-section") {
        const section = buildMenuSection({
            id,
            label
        });

        element.replaceWith(section);
        return;
    }

    if (resource.type === "menu-item") {
        const item = buildMenuItem({
            id,
            label,
            icon: resource.icon,
            route: resource.route,
            active: resource.active
        });

        element.replaceWith(item);
        return;
    }

    console.warn(`Unsupported UI resource type: ${resource.type}`);
}

function renderUi() {
    document.documentElement.lang = getLocale();

    const elements = document.querySelectorAll("[data-ui-id]");

    elements.forEach(renderUiResource);
}

function setBridgeStatus(resourceId) {
    const resource = getUiResource(resourceId);

    if (!resource) {return;}

    const text = translate(resource.textKey);
    const connectionStatus = document.querySelector('[data-ui-id="bridge.connecting"]');

    if (connectionStatus) {
        connectionStatus.textContent = text;
        connectionStatus.dataset.uiId = resourceId;
    }
}

renderUi();

connectBridge()
    .then((bridge) => {
        const name = bridge.applicationName;
        const version = bridge.applicationVersion;
        const connectionStatus = document.querySelector('[data-ui-id="bridge.connecting"]');

        connectionDot.classList.add("ready");
        appVersion.textContent = `${name} ${version}`;

        if (connectionStatus) {
            connectionStatus.textContent = translate("bridge.connected");
            connectionStatus.dataset.uiId = "bridge.connected";
        }
    })
    .catch((error) => {
        console.error(error);

        const connectionStatus = document.querySelector('[data-ui-id="bridge.connecting"]');

        if (connectionStatus) {
            connectionStatus.textContent = translate("bridge.error");
            connectionStatus.dataset.uiId = "bridge.error";
        }
    });