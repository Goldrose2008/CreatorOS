CREATE TABLE IF NOT EXISTS content_types (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE
);

CREATE TABLE IF NOT EXISTS contents (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    project_id INTEGER NOT NULL
        REFERENCES projects(id)
        ON DELETE CASCADE,
    content_type_id INTEGER NOT NULL
        REFERENCES content_types(id)
        ON DELETE RESTRICT,
    content_role TEXT NOT NULL
        CHECK (content_role IN ('main', 'additional')),
    name TEXT NOT NULL
);

CREATE UNIQUE INDEX IF NOT EXISTS ux_contents_main_per_project
    ON contents(project_id)
    WHERE content_role = 'main';

INSERT OR IGNORE INTO content_types (name)
VALUES
    ('Видео'),
    ('Short'),
    ('Статья');