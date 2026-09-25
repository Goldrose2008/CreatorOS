import Database from "@tauri-apps/plugin-sql";
import { invoke } from "@tauri-apps/api/core";
import { DATABASE_SCHEMA_VERSION } from "../config/databaseSchema";

const DATABASE_NAME = "sqlite:creator.db";
const SCHEMA_VERSION_KEY = "creatoros_database_schema_version";

let databasePromise: Promise<Database> | null = null;

async function getDatabaseSchemaVersion(database: Database): Promise<number> {
    const result = await database.select<{ user_version: number }[]>("PRAGMA user_version");
    return Number(result[0]?.user_version ?? 0);
}

async function initializeDatabase(): Promise<Database> {
    const storedVersion = Number(localStorage.getItem(SCHEMA_VERSION_KEY) ?? "0");

    if (storedVersion !== DATABASE_SCHEMA_VERSION) {
        console.log(
            `Версия схемы приложения ${DATABASE_SCHEMA_VERSION}, ` +
            `сохранённая версия ${storedVersion}. ` +
            `База данных будет пересоздана.`
        );

        await invoke("reset_database");
    }

    const database = await Database.load(DATABASE_NAME);

    const actualVersion = await getDatabaseSchemaVersion(database);

    if (actualVersion !== DATABASE_SCHEMA_VERSION) {
        await database.close();

        throw new Error(
            `Версия созданной БД ${actualVersion} ` +
            `не соответствует ожидаемой версии ` +
            `${DATABASE_SCHEMA_VERSION}.`
        );
    }

    localStorage.setItem(SCHEMA_VERSION_KEY, String(DATABASE_SCHEMA_VERSION));

    return database;
}

export async function getDatabase(): Promise<Database> {
    if (!databasePromise) {
        databasePromise = initializeDatabase();
    }

    return databasePromise;
}

export async function checkProjectsTable(): Promise<boolean> {
    const database = await getDatabase();

    const result = await database.select<{ name: string }[]>(
        `SELECT name
        FROM sqlite_master
        WHERE type = 'table'
        AND name = ?`,
        ["projects"]
    );

    return result.length > 0;
}