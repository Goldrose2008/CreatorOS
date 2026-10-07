CREATE TABLE IF NOT EXISTS tasks (
    id INTEGER PRIMARY KEY AUTOINCREMENT,

    parent_id INTEGER NOT NULL,

    parent_type TEXT NOT NULL
        CHECK (parent_type IN ('project', 'content')),

    parent_task_id INTEGER
        REFERENCES tasks(id)
        ON DELETE CASCADE,

    name TEXT NOT NULL,

    description TEXT NOT NULL DEFAULT '',

    deadline_at TEXT NOT NULL DEFAULT '',

    status TEXT NOT NULL DEFAULT 'todo'
        CHECK (
            status IN (
                'todo',
                'in_progress',
                'done',
                'cancelled'
            )
        ),

    priority INTEGER NOT NULL DEFAULT 0,

    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,

    updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX IF NOT EXISTS ix_tasks_parent
    ON tasks(parent_type, parent_id);

CREATE INDEX IF NOT EXISTS ix_tasks_parent_task
    ON tasks(parent_task_id);