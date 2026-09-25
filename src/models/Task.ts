export interface Task {
    id: number;
    parent_id: number;
    parent_type: string;
    parent_task_id?: number;
    title: string;
    description?: string;
    status: string;
    assigned_user_id?: number;
    start_date?: string;
    due_date?: string;
    completed_at?: string;
    created_at: string;
    updated_at: string;
}