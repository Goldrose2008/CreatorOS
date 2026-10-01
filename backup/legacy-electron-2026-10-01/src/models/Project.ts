export type ProjectStatus =
    | "draft"
    | "active"
    | "archived";

export interface Project {
    id: number;
    name: string;
    description?: string;
    owner_id?: number | null;
    planned_release_at: string;
    status: ProjectStatus;
    progress: number;
    created_at: string;
    updated_at: string;
}