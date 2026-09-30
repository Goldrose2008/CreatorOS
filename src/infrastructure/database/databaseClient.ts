export type SqlValue = string | number | boolean | null;

export interface DatabaseClient {
    select<T = unknown>(
        sql: string,
        params?: SqlValue[]
    ): Promise<T[]>;

    execute(
        sql: string,
        params?: SqlValue[]
    ): Promise<{
        changes: number;
        lastInsertId?: number;
    }>;
}

class ElectronDatabaseClient implements DatabaseClient {
    public select<T = unknown>(
        sql: string,
        params: SqlValue[] = []
    ): Promise<T[]> {
        return window.creatorOS.database.select<T>(
            sql,
            params
        );
    }

    public execute(
        sql: string,
        params: SqlValue[] = []
    ): Promise<{
        changes: number;
        lastInsertId?: number;
    }> {
        return window.creatorOS.database.execute(
            sql,
            params
        );
    }
}

const databaseClient = new ElectronDatabaseClient();

export function getDatabaseClient(): DatabaseClient {
    if (!window.creatorOS?.database) {
        throw new Error(
            "Electron database API недоступен."
        );
    }

    return databaseClient;
}
