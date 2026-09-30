import type { Content, ContentRole } from "../models/Content";
import { getDatabaseClient } from "../infrastructure/database/databaseClient";

export async function getProjectContent(projectId: number): Promise<Content[]> {
    const database = await getDatabaseClient();

    return database.select<Content>(
        `SELECT
            content_items.*,
            projects.planned_release_at AS planned_release_at
        FROM content_items
        INNER JOIN projects
            ON projects.id = content_items.project_id
        WHERE content_items.project_id = ?
        ORDER BY
            CASE content_items.content_role
                WHEN 'main' THEN 0
                ELSE 1
            END,
            content_items.created_at ASC,
            content_items.id ASC`,
        [projectId]
    );
}

export async function getContentById(id: number): Promise<Content | null> {
    const database = await getDatabaseClient();
    const content = await database.select<Content>(
        `SELECT
            content_items.*,
            projects.planned_release_at AS planned_release_at
        FROM content_items
        INNER JOIN projects
            ON projects.id = content_items.project_id
        WHERE content_items.id = ?`,
        [id]
    );

    if (content.length === 0) { return null; }

    return content[0];
}

export async function createContent(
    projectId: number,
    contentTypeId: number,
    contentRole: ContentRole,
    name: string,
    description: string
): Promise<void> {
    const database = await getDatabaseClient();

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
    const database = await getDatabaseClient();

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
    const database = await getDatabaseClient();

    await database.execute(
        `DELETE FROM content_items
        WHERE id = ?`,
        [id]
    );
}