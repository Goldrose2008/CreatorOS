import type { Project } from "../models/Project";
import { getDatabase } from "./databaseService";

export async function getProjects(): Promise<Project[]> {
    const database = await getDatabase();
    const projects = await database.select<Project[]>(
        `SELECT *
        FROM projects
        ORDER BY created_at DESC`
    );
    return projects;
}

export async function getProjectById(id: number): Promise<Project | null> {
    const database = await getDatabase();
    const projects = await database.select<Project[]>(
        `SELECT *
        FROM projects
        WHERE id = ?`,
        [id]
    );

    if (projects.length === 0) {
        return null;
    }

    return projects[0];
}

export async function createProject(
    name: string, 
    description: string,
    plannedReleaseAt: string
): Promise<void> {
    const database = await getDatabase();

    await database.execute(
        `INSERT INTO projects(
            name, 
            description,
            planned_release_at
        )
        VALUES(?, ?, ?)`,
        [
            name,
            description,
            plannedReleaseAt
        ]
    );
}

export async function updateProject(
    id: number, 
    name: string, 
    description: string, 
    plannedReleaseAt: string
): Promise<void> {
    const database = await getDatabase();

    await database.execute(
        `UPDATE projects
        SET
            name = ?,
            description = ?,
            planned_release_at = ?,
            updated_at = CURRENT_TIMESTAMP
        WHERE id = ?`,
        [
            name, 
            description, 
            plannedReleaseAt,
            id
        ]
    );
}

export async function updateProjectStatus(
    id: number,
    status: string
): Promise<void> {
    const database = await getDatabase();

    await database.execute(
        `UPDATE projects
        SET
            status = ?,
            updated_at = CURRENT_TIMESTAMP
        WHERE id = ?`,
        [
            status,
            id
        ]
    );
}

export async function deleteProject(id: number): Promise<void> {
    const database = await getDatabase();

    await database.execute(
        `DELETE FROM projects
        WHERE id = ?`,
        [id]
    );
}
