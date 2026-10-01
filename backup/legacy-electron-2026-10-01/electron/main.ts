import { app, BrowserWindow } from "electron";
import path from "node:path";
import { fileURLToPath } from "node:url";
import { ElectronDatabase } from "./database/database.js";
import { registerDatabaseHandlers } from "./ipc/databaseHandlers.js";

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

const DEVELOPMENT_URL = process.env.ELECTRON_RENDERER_URL;

let database: ElectronDatabase | null = null;

function getDatabasePath(): string {
    return path.join(
        app.getPath("userData"),
        "creator.db"
    );
}

function getSchemaPath(): string {
    return path.join(
        app.getAppPath(),
        "electron",
        "database",
        "schema.sql"
    );
}

function createWindow(): void {
    const window = new BrowserWindow({
        width: 1280,
        height: 800,
        minWidth: 1000,
        minHeight: 650,
        webPreferences: {
            preload: path.join(
                __dirname,
                "preload.cjs"
            ),
            contextIsolation: true,
            nodeIntegration: false,
            sandbox: true,
        },
    });

    window.webContents.setWindowOpenHandler(() => ({
        action: "deny",
    }));

    if (DEVELOPMENT_URL) {
        void window.loadURL(DEVELOPMENT_URL);
        return;
    }

    void window.loadFile(
        path.join(
            app.getAppPath(),
            "dist",
            "index.html"
        )
    );
}

app.whenReady().then(() => {
    database = new ElectronDatabase(
        getSchemaPath()
    );

    database.initialize(
        getDatabasePath()
    );

    registerDatabaseHandlers(database);
    createWindow();

    app.on("activate", () => {
        if (BrowserWindow.getAllWindows().length === 0) {
            createWindow();
        }
    });
});

app.on("window-all-closed", () => {
    if (process.platform !== "darwin") {
        app.quit();
    }
});

app.on("before-quit", () => {
    database?.close();
    database = null;
});
