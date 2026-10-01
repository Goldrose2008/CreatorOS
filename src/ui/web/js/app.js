import { connectBridge } from "./bridge.js";

const status = document.getElementById("status");

connectBridge()
    .then((bridge) => {
        const name = bridge.applicationName;
        const version = bridge.applicationVersion;

        status.textContent = `${name} ${version} — C++ ↔ WebChannel работает`;
        status.classList.add("ready");
    })
    .catch((error) => {
        console.error(error);
        status.textContent = "Ошибка подключения к C++ WebBridge";
    });