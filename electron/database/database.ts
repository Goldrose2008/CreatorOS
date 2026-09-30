import { existsSync, readFileSync } from "node:fs";
import { DatabaseSync } from "node:sqlite";

export const DATABASE_SCHEMA_VERSION = 5;

export type SqlValue = string | number | boolean | null;

export interface DatabaseRequest {
    sql: string;
    params: SqlValue[];
}

export class ElectronDatabase {
    private database: DatabaseSync | null = null;

    public constructor(
        private readonly schemaPath: string
    ) {}

    public initialize(databasePath: string): void {
        const databaseExists = existsSync(databasePath);

        this.database = new DatabaseSync(databasePath, {
            timeout: 5000,
        });

        const currentVersionRow = this.database
            .prepare("PRAGMA user_version")
            .get() as { user_version?: number };

        const currentVersion = Number(
            currentVersionRow?.user_version ?? 0
        );

        if (!databaseExists || currentVersion === 0) {
            const schema = readFileSync(
                this.schemaPath,
                "utf8"
            );

            this.database.exec(schema);
            return;
        }

        if (currentVersion !== DATABASE_SCHEMA_VERSION) {
            this.close();

            throw new Error(
                `Версия БД ${currentVersion} не поддерживается. Ожидается ${DATABASE_SCHEMA_VERSION}.`
            );
        }
    }

    public select(request: DatabaseRequest): unknown[] {
        const statement = this.getConnection()
            .prepare(request.sql);

        return statement.all(
            ...this.normalizeSqlParams(request.params)
        );
    }

    public execute(request: DatabaseRequest): {
        changes: number;
        lastInsertId?: number;
    } {
        const statement = this.getConnection()
            .prepare(request.sql);

        const result = statement.run(
            ...this.normalizeSqlParams(request.params)
        );

        return {
            changes: Number(result.changes),
            lastInsertId: Number(result.lastInsertRowid),
        };
    }

    public close(): void {
        this.database?.close();
        this.database = null;
    }

    private getConnection(): DatabaseSync {
        if (!this.database) {
            throw new Error(
                "База данных ещё не инициализирована."
            );
        }

        return this.database;
    }

    private normalizeSqlParams(
        params: SqlValue[]
    ): Array<string | number | null> {
        return params.map((value) =>
            typeof value === "boolean"
                ? Number(value)
                : value
        );
    }
}
