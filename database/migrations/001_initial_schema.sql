
PRAGMA foreign_keys = ON;

CREATE TABLE object_types (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uuid TEXT NOT NULL UNIQUE,
    name TEXT NOT NULL,
    description TEXT,
    created_at TEXT NOT NULL,
    updated_at TEXT NOT NULL
);

CREATE UNIQUE INDEX ux_object_types_name
    ON object_types(name COLLATE NOCASE);


CREATE TABLE objects (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uuid TEXT NOT NULL UNIQUE,
    object_type_id INTEGER,
    name TEXT NOT NULL,
    description TEXT,
    deadline_at TEXT,
    progress REAL,
    created_at TEXT NOT NULL,
    updated_at TEXT NOT NULL,
    deleted_at TEXT,
    former_object_type_uuid TEXT,
    former_object_type_name TEXT,
    deleted_timezone TEXT,

    FOREIGN KEY (object_type_id)
        REFERENCES object_types(id)
        ON DELETE SET NULL,

    CHECK (progress IS NULL OR (progress >= 0 AND progress <= 100)),
    CHECK (object_type_id IS NOT NULL OR deleted_at IS NOT NULL),
    CHECK 
    (
        (object_type_id IS NOT NULL AND former_object_type_uuid IS NULL AND former_object_type_name IS NULL)
        OR
        (object_type_id IS NULL AND deleted_at IS NOT NULL AND former_object_type_uuid IS NOT NULL AND former_object_type_name IS NOT NULL)
    ),

    CHECK ((deleted_at IS NULL AND deleted_timezone IS NULL) OR (deleted_at IS NOT NULL AND deleted_timezone IS NOT NULL))
);

CREATE UNIQUE INDEX ux_objects_active_name
    ON objects(name COLLATE NOCASE)
    WHERE deleted_at IS NULL;

CREATE INDEX ix_objects_object_type_id
    ON objects(object_type_id);

CREATE INDEX ix_objects_deleted_at
    ON objects(deleted_at);


CREATE TABLE object_relations (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uuid TEXT NOT NULL UNIQUE,
    parent_object_id INTEGER NOT NULL,
    child_object_id INTEGER NOT NULL,
    role TEXT NOT NULL,
    created_at TEXT NOT NULL,
    updated_at TEXT NOT NULL,

    FOREIGN KEY (parent_object_id)
        REFERENCES objects(id)
        ON DELETE CASCADE,

    FOREIGN KEY (child_object_id)
        REFERENCES objects(id)
        ON DELETE CASCADE,

    CHECK (parent_object_id <> child_object_id),
    CHECK (role IN ('main', 'addition', 'support')),

    UNIQUE (parent_object_id, child_object_id)
);

CREATE INDEX ix_object_relations_child_object_id
    ON object_relations(child_object_id);


CREATE TABLE property_definitions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uuid TEXT NOT NULL UNIQUE,
    scope TEXT NOT NULL,
    object_type_id INTEGER,
    name TEXT NOT NULL,
    description TEXT,
    value_type TEXT NOT NULL,
    is_multiple INTEGER NOT NULL DEFAULT 0,
    is_required INTEGER NOT NULL DEFAULT 0,
    default_value TEXT,
    is_enabled INTEGER NOT NULL DEFAULT 1,
    show_on_card INTEGER NOT NULL DEFAULT 0,
    created_at TEXT NOT NULL,
    updated_at TEXT NOT NULL,

    FOREIGN KEY (object_type_id)
        REFERENCES object_types(id)
        ON DELETE CASCADE,

    CHECK (scope IN ('common', 'object_type')),

    CHECK ((scope = 'common' AND object_type_id IS NULL) OR (scope = 'object_type' AND object_type_id IS NOT NULL)),
    CHECK (value_type IN ('text', 'number', 'boolean', 'date', 'datetime', 'entity_reference')),
    CHECK (is_multiple IN (0, 1)),
    CHECK (is_required IN (0, 1)),
    CHECK (is_enabled IN (0, 1)),
    CHECK (show_on_card IN (0, 1))
);

CREATE UNIQUE INDEX ux_property_definitions_common_name
    ON property_definitions(name COLLATE NOCASE)
    WHERE scope = 'common';

CREATE UNIQUE INDEX ux_property_definitions_object_type_name
    ON property_definitions(object_type_id, name COLLATE NOCASE)
    WHERE scope = 'object_type';

CREATE INDEX ix_property_definitions_object_type_id
    ON property_definitions(object_type_id);


CREATE TABLE property_text_settings (
    property_id INTEGER PRIMARY KEY,
    min_length INTEGER,
    max_length INTEGER,

    FOREIGN KEY (property_id)
        REFERENCES property_definitions(id)
        ON DELETE CASCADE,

    CHECK (min_length IS NULL OR min_length >= 0),
    CHECK (max_length IS NULL OR max_length >= 0),
    CHECK (min_length IS NULL OR max_length IS NULL OR min_length <= max_length)
);


CREATE TABLE property_entity_reference_settings (
    property_id INTEGER PRIMARY KEY,
    target_entity_type TEXT NOT NULL,

    FOREIGN KEY (property_id)
        REFERENCES property_definitions(id)
        ON DELETE CASCADE
);


CREATE TABLE property_options (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    property_id INTEGER NOT NULL,
    value TEXT NOT NULL,
    normalized_value TEXT NOT NULL,

    FOREIGN KEY (property_id)
        REFERENCES property_definitions(id)
        ON DELETE CASCADE,

    UNIQUE (property_id, normalized_value)
);


CREATE TABLE property_values (
    object_id INTEGER NOT NULL,
    property_id INTEGER NOT NULL,
    value TEXT NOT NULL,

    PRIMARY KEY (object_id, property_id),

    FOREIGN KEY (object_id)
        REFERENCES objects(id)
        ON DELETE CASCADE,

    FOREIGN KEY (property_id)
        REFERENCES property_definitions(id)
        ON DELETE CASCADE
);

CREATE INDEX ix_property_values_property_id
    ON property_values(property_id);
