use std::fs;
use tauri::Manager;

#[tauri::command]
fn reset_database(app: tauri::AppHandle) -> Result<(), String> {
    let app_config_dir = app
        .path()
        .app_config_dir()
        .map_err(|error| format!("Не удалось определить папку БД: {error}"))?;

    let database_files = [
        "creator.db",
        "creator.db-wal",
        "creator.db-shm",
        "creator.db-journal",
    ];

    for file_name in database_files {
        let file_path = app_config_dir.join(file_name);

        match fs::remove_file(&file_path) {
            Ok(()) => {
                println!("Удалён файл БД: {:?}", file_path);
            }
            Err(error) if error.kind() == std::io::ErrorKind::NotFound => {
                // Файл отсутствует — это нормально.
            }
            Err(error) => {
                return Err(format!("Не удалось удалить файл БД {:?}: {error}", file_path));
            }
        }
    }
    Ok(())
}

#[cfg_attr(mobile, tauri::mobile_entry_point)]
pub fn run() {
    tauri::Builder::default()
        .plugin(
            tauri_plugin_log::Builder::default()
                .build(),
        )
        .plugin(
            tauri_plugin_sql::Builder::default()
                .add_migrations(
                    "sqlite:creator.db",
                    vec![
                        tauri_plugin_sql::Migration {
                            version: 1,
                            description: "create projects table",
                            sql: include_str!("../migrations/001_initial.sql"),
                            kind: tauri_plugin_sql::MigrationKind::Up,
                        },
                    ],
                )
                .build(),
        )
        .invoke_handler(tauri::generate_handler![reset_database])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}