CREATE TABLE IF NOT EXISTS projects (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    description TEXT,
    owner_id INTEGER,
    planned_release_at TEXT NOT NULL,
    status TEXT NOT NULL DEFAULT 'draft',
    progress INTEGER NOT NULL DEFAULT 0,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS content_types (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE,
    description TEXT,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

INSERT OR IGNORE INTO content_types (name, description)
VALUES
    ('Видео', 'Основной видеоконтент.'),
    ('Short', 'Короткий формат видеоконтента.'),
    ('Статья', 'Текстовый контент.');

CREATE TABLE IF NOT EXISTS content_items (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    project_id INTEGER NOT NULL,
    content_type_id INTEGER NOT NULL,
    content_role TEXT NOT NULL,
    name TEXT NOT NULL,
    description TEXT,
    priority INTEGER NOT NULL DEFAULT 0,
    production_deadline_at TEXT,
    status TEXT NOT NULL DEFAULT 'draft',
    progress INTEGER NOT NULL DEFAULT 0,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE UNIQUE INDEX IF NOT EXISTS ux_content_items_main_per_project
ON content_items(project_id)
WHERE content_role = 'main';

PRAGMA user_version = 5;
