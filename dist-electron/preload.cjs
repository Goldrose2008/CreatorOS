"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const electron_1 = require("electron");
electron_1.contextBridge.exposeInMainWorld("creatorOS", {
    database: {
        select(sql, params = []) {
            return electron_1.ipcRenderer.invoke("database:select", { sql, params });
        },
        execute(sql, params = []) {
            return electron_1.ipcRenderer.invoke("database:execute", { sql, params });
        },
    },
});
//# sourceMappingURL=preload.cjs.map