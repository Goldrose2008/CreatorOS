export type ContentRole = "main" | "additional";

export interface Content {
    id: number;
    project_id: number;
    content_type_id: number;
    content_role: ContentRole;
    name: string;
    description?: string;
    priority: number;
    production_deadline_at?: string;
    status: string;
    progress: number;
    created_at: string;
    updated_at: string;
}