import Database from "@tauri-apps/plugin-sql";
import { DATABASE_SCHEMA } from "../config/databaseSchema";

let db: Database | null = null;

async function ensureDatabaseSchema(database: Database): Promise<void> {
    for (const [tableName, columns] of Object.entries(DATABASE_SCHEMA)) {
        const tableExists =await database.select<{ name: string }[]>(
                `SELECT name
                FROM sqlite_master
                WHERE type = 'table'
                AND name = ?`,
                [tableName]
            );

        if (tableExists.length === 0) {
            continue;
        }

        const existingColumns = await database.select<{ name: string }[]>(
                `PRAGMA table_info(${tableName})`
            );

        const existingColumnNames =new Set(existingColumns.map(column => column.name));

        for (const [columnName, columnDefinition] of Object.entries(columns)) {
            if (existingColumnNames.has(columnName)) {
                continue;
            }

            await database.execute(
                `ALTER TABLE ${tableName}
                ADD COLUMN ${columnName} ${columnDefinition}`
            );
        }
    }
}

export async function getDatabase(): Promise<Database> {
    if (!db) {
        db = await Database.load("sqlite:creator.db");
        await ensureDatabaseSchema(db);
    }
    return db;
}

export async function checkProjectsTable(): Promise<boolean> {
    const database = await getDatabase();
    const result = await database.select<{ name: string }[]>(
        `SELECT name
        FROM sqlite_master
        WHERE type='table'
        AND name=?`,
        ["projects"]
    );
    return result.length > 0;
}