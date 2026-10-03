#pragma once

#include <QMainWindow>
#include <memory>

#include "../infrastructure/database/DatabaseManager.h"

class ProjectRepository;
class ProjectService;

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    
    ~MainWindow() override;

private:
    DatabaseManager databaseManager_;
    std::unique_ptr<ProjectRepository> projectRepository_;
    std::unique_ptr<ProjectService> projectService_;
};