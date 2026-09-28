import type { Content, ContentRole } from "../models/Content";
import { getDatabase } from "./databaseService";

export async function getProjectContent(projectId: number): Promise<Content[]> {
    const database = await getDatabase();

    return database.select<Content[]>(
        `SELECT *
        FROM content_items
        WHERE project_id = ?
        ORDER BY
            CASE content_role
                WHEN 'main' THEN 0
                ELSE 1
            END,
            created_at ASC,
            id ASC`,
        [projectId]
    );
}

export async function createContent(
    projectId: number,
    contentTypeId: number,
    contentRole: ContentRole,
    name: string,
    description: string
): Promise<void> {
    const database = await getDatabase();

    await database.execute(
        `INSERT INTO content_items(
            project_id,
            content_type_id,
            content_role,
            name,
            description
        )
        VALUES(?, ?, ?, ?, ?)`,
        [
            projectId,
            contentTypeId,
            contentRole,
            name,
            description,
        ]
    );
}

export async function updateContent(
    id: number,
    contentTypeId: number,
    name: string,
    description: string
): Promise<void> {
    const database = await getDatabase();

    await database.execute(
        `UPDATE content_items
        SET
            content_type_id = ?,
            name = ?,
            description = ?,
            updated_at = CURRENT_TIMESTAMP
        WHERE id = ?`,
        [
            contentTypeId,
            name,
            description,
            id,
        ]
    );
}

export async function deleteContent(id: number): Promise<void> {
    const database = await getDatabase();

    await database.execute(
        `DELETE FROM content_items
        WHERE id = ?`,
        [id]
    );
}