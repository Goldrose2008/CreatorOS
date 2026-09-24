export interface Project {
    id: number;
    name: string;
    description?: string;
    project_type: string;
    status: string;
    planned_publication_date?: string;
    owner_id?: number;
    created_at: string;
    updated_at: string;
}