ALTER TABLE contents
ADD COLUMN description TEXT NOT NULL DEFAULT '';

ALTER TABLE contents
ADD COLUMN priority INTEGER NOT NULL DEFAULT 0;

ALTER TABLE contents
ADD COLUMN production_deadline_at TEXT NOT NULL DEFAULT '';

ALTER TABLE contents
ADD COLUMN status TEXT NOT NULL DEFAULT 'draft'
    CHECK (status IN ('draft', 'in_progress', 'ready', 'archived'));

ALTER TABLE contents
ADD COLUMN progress INTEGER NOT NULL DEFAULT 0
    CHECK (progress BETWEEN 0 AND 100);

ALTER TABLE contents
ADD COLUMN created_at TEXT NOT NULL DEFAULT '';

ALTER TABLE contents
ADD COLUMN updated_at TEXT NOT NULL DEFAULT '';

UPDATE contents
SET created_at = CURRENT_TIMESTAMP
WHERE created_at = '';

UPDATE contents
SET updated_at = CURRENT_TIMESTAMP
WHERE updated_at = '';