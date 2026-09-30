import { app, BrowserWindow, ipcMain } from "electron";
import { readFileSync } from "node:fs";
import { existsSync } from "node:fs";
import path from "node:path";
import { fileURLToPath } from "node:url";
import { DatabaseSync } from "node:sqlite";
const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const DATABASE_SCHEMA_VERSION = 5;
const DEVELOPMENT_URL = process.env.ELECTRON_RENDERER_URL;
let database = null;
function getDatabasePath() {
    return path.join(app.getPath("userData"), "creator.db");
}
function getDatabase() {
    if (!database) {
        throw new Error("База данных ещё не инициализирована.");
    }
    return database;
}
function initializeDatabase() {
    const databasePath = getDatabasePath();
    const databaseExists = existsSync(databasePath);
    database = new DatabaseSync(databasePath, {
        timeout: 5000,
    });
    const currentVersionRow = database
        .prepare("PRAGMA user_version")
        .get();
    const currentVersion = Number(currentVersionRow?.user_version ?? 0);
    if (!databaseExists || currentVersion === 0) {
        const schemaPath = path.join(app.getAppPath(), "electron", "database", "schema.sql");
        const schema = readFileSync(schemaPath, "utf8");
        database.exec(schema);
        return;
    }
    if (currentVersion !== DATABASE_SCHEMA_VERSION) {
        database.close();
        database = null;
        throw new Error(`Версия БД ${currentVersion} не поддерживается. Ожидается ${DATABASE_SCHEMA_VERSION}.`);
    }
}
function normalizeSqlParams(params) {
    return params.map((value) => typeof value === "boolean" ? Number(value) : value);
}
function registerDatabaseHandlers() {
    ipcMain.handle("database:select", (_event, request) => {
        const statement = getDatabase().prepare(request.sql);
        return statement.all(...normalizeSqlParams(request.params));
    });
    ipcMain.handle("database:execute", (_event, request) => {
        const statement = getDatabase().prepare(request.sql);
        const result = statement.run(...normalizeSqlParams(request.params));
        return {
            changes: Number(result.changes),
            lastInsertId: Number(result.lastInsertRowid),
        };
    });
}
function createWindow() {
    const window = new BrowserWindow({
        width: 1280,
        height: 800,
        minWidth: 1000,
        minHeight: 650,
        webPreferences: {
            preload: path.join(__dirname, "preload.cjs"),
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
    void window.loadFile(path.join(app.getAppPath(), "dist", "index.html"));
}
app.whenReady().then(() => {
    initializeDatabase();
    registerDatabaseHandlers();
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
//# sourceMappingURL=main.js.map