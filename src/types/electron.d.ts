type SqlValue = string | number | boolean | null;

interface CreatorOSDatabaseAPI {
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

interface CreatorOSElectronAPI {
    database: CreatorOSDatabaseAPI;
}

declare global {
    interface Window {
        creatorOS: CreatorOSElectronAPI;
    }
}

export {};
