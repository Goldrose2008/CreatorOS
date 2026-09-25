export interface Project {
    id: number;
    name: string;
    description?: string;
    owner_id?: number | null;
    planned_release_at?: string | null;
    status: string;
    progress: number;
    created_at: string;
    updated_at: string;
}