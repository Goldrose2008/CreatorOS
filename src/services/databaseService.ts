interface DatabaseClient {
    select<T = unknown>(
        sql: string,
        params?: Array<string | number | boolean | null>
    ): Promise<T[]>;

    execute(
        sql: string,
        params?: Array<string | number | boolean | null>
    ): Promise<{
        changes: number;
        lastInsertId?: number;
    }>;
}

export async function getDatabase(): Promise<DatabaseClient> {
    if (!window.creatorOS?.database) {
        throw new Error("Electron database API недоступен.");
    }

    return window.creatorOS.database;
}

export async function checkProjectsTable(): Promise<boolean> {
    const database = await getDatabase();

    const result = await database.select<{ name: string }>(
        \`SELECT name
        FROM sqlite_master
        WHERE type = 'table'
        AND name = ?\`,
        ["projects"]
    );

    return result.length > 0;
}
