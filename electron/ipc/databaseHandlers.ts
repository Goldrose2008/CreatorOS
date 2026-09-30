import { ipcMain } from "electron";
import type { ElectronDatabase, DatabaseRequest } from "../database/database.js";

export function registerDatabaseHandlers(
    database: ElectronDatabase
): void {
    ipcMain.handle(
        "database:select",
        (_event, request: DatabaseRequest) =>
            database.select(request)
    );

    ipcMain.handle(
        "database:execute",
        (_event, request: DatabaseRequest) =>
            database.execute(request)
    );
}
