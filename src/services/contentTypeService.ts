import type { ContentType } from "../models/ContentType";
import { getDatabase } from "./databaseService";

export async function getContentTypes(): Promise<ContentType[]> {
    const database = await getDatabase();
    const contentTypes = await database.select<ContentType>(
        `SELECT *
        FROM content_types
        ORDER BY created_at ASC, id ASC`
    );

    return contentTypes;
}

export async function createContentType(
    name: string,
    description: string
): Promise<void> {
    const database = await getDatabase();

    await database.execute(
        `INSERT INTO content_types(
            name,
            description
        )
        VALUES(?, ?)`,
        [
            name,
            description,
        ]
    );
}

export async function updateContentType(
    id: number,
    name: string,
    description: string
): Promise<void> {
    const database = await getDatabase();

    await database.execute(
        `UPDATE content_types
        SET
            name = ?,
            description = ?,
            updated_at = CURRENT_TIMESTAMP
        WHERE id = ?`,
        [
            name,
            description,
            id,
        ]
    );
}

export async function deleteContentType(id: number): Promise<void> {
    const database = await getDatabase();

    await database.execute(
        `DELETE FROM content_types
        WHERE id = ?`,
        [id]
    );
}