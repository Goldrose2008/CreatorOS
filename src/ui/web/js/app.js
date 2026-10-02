import { connectBridge } from "./bridge.js";
import { buildMainMenu } from "./menu/main-menu.js";
import {
    initializeLocalization,
    translate
} from "./i18n/i18n.js";

import {
    buildBrandMark,
    buildBrandName
} from "./components/brand.js";

const connectionDot = document.getElementById("connection-dot");
const connectionStatus = document.getElementById("connection-status");
const appVersion = document.getElementById("app-version");

function renderTextResources(applicationName) {
    const elements = document.querySelectorAll("[data-text-id]");

    elements.forEach(element => {
        const textId = element.dataset.textId;

        element.textContent = translate(textId, {appName: applicationName});
    });
}

function renderApplication(bridge) {
    document.title = bridge.applicationName;

    const brandMark = buildBrandMark({
            id: "app-brand-mark",
            name: bridge.applicationName
        });

    document
        .getElementById("app-brand-mark")
        .replaceWith(brandMark);

    const brandName = buildBrandName({
            id: "app-brand-name",
            name: bridge.applicationName
        });

    document
        .getElementById("app-brand-name")
        .replaceWith(brandName);

    appVersion.textContent = `${bridge.applicationName} ${bridge.applicationVersion}`;

    const navigation = document.getElementById("main-navigation");

    navigation.setAttribute("aria-label", translate("main_navigation"));
    buildMainMenu();
    renderTextResources(bridge.applicationName);
    connectionDot.classList.add("ready");
    connectionStatus.textContent = translate("bridge.connected");
}

async function initializeApplication() {
    try {
        await initializeLocalization();
        connectionStatus.textContent = translate("bridge.connecting");

        const bridge = await connectBridge();
        renderApplication(bridge);

    } 
    catch (error) {
        console.error(error);
        if (connectionStatus) {connectionStatus.textContent =translate("bridge.error");}
    }
}

initializeApplication();