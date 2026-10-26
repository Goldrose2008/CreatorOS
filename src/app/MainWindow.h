#pragma once

#include <QMainWindow>
#include <memory>

#include "../infrastructure/database/DatabaseManager.h"

class AccountRepository;
class AccountService;
class ContentTypeRepository;
class ContentRepository;
class ContentService;
class PlatformRepository;
class PlatformService;
class ProjectRepository;
class ProjectService;
class TaskRepository;
class TaskService;

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    
    ~MainWindow() override;

private:
    DatabaseManager databaseManager_;
    
    std::unique_ptr<AccountRepository> accountRepository_;
    std::unique_ptr<ContentRepository> contentRepository_;
    std::unique_ptr<ContentTypeRepository> contentTypeRepository_;
    std::unique_ptr<PlatformRepository> platformRepository_;
    std::unique_ptr<ProjectRepository> projectRepository_;
    std::unique_ptr<TaskRepository> taskRepository_;

    std::unique_ptr<AccountService> accountService_;
    std::unique_ptr<ContentService> contentService_;
    std::unique_ptr<PlatformService> platformService_;
    std::unique_ptr<ProjectService> projectService_;
    std::unique_ptr<TaskService> taskService_;
};