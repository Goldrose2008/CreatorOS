import { contextBridge, ipcRenderer } from "electron";

type SqlValue = string | number | boolean | null;

interface DatabaseRequest {
    sql: string;
    params: SqlValue[];
}

contextBridge.exposeInMainWorld("creatorOS", {
    database: {
        select<T = unknown>(
            sql: string,
            params: SqlValue[] = []
        ): Promise<T[]> {
            return ipcRenderer.invoke(
                "database:select",
                { sql, params } satisfies DatabaseRequest
            );
        },

        execute(
            sql: string,
            params: SqlValue[] = []
        ): Promise<{
            changes: number;
            lastInsertId?: number;
        }> {
            return ipcRenderer.invoke(
                "database:execute",
                { sql, params } satisfies DatabaseRequest
            );
        },
    },
});
