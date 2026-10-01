import { connectBridge } from "./bridge.js";

const connectionStatus = document.getElementById("connection-status");
const connectionDot = document.getElementById("connection-dot");
const appVersion = document.getElementById("app-version");

connectBridge()
    .then((bridge) => {
        const name = bridge.applicationName;
        const version = bridge.applicationVersion;

        connectionStatus.textContent = "C++ bridge подключён";
        connectionDot.classList.add("ready");
        appVersion.textContent = `${name} ${version}`;
    })
    .catch((error) => {
        console.error(error);
        connectionStatus.textContent = "Ошибка подключения";
    });