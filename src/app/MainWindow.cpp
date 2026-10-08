#include "MainWindow.h"

#include <QMessageBox>

#include "AppInfo.h"
#include "../ui/shell/AppShell.h"
#include "../application/content/ContentService.h"
#include "../application/projects/ProjectService.h"
#include "../application/tasks/TaskService.h"
#include "../infrastructure/database/ContentTypeRepository.h"
#include "../infrastructure/database/ContentRepository.h"
#include "../infrastructure/database/ProjectRepository.h"
#include "../infrastructure/database/TaskRepository.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle(QString::fromLatin1(AppInfo::Name));
    resize(1280, 800);
    QString databaseError;

    if (!databaseManager_.initialize(&databaseError))
    {
        QMessageBox::critical(this, QStringLiteral("CreatorOS"), databaseError);
        return;
    }

    contentTypeRepository_ = std::make_unique<ContentTypeRepository>(databaseManager_);
    contentRepository_ = std::make_unique<ContentRepository>(databaseManager_);
    projectRepository_ = std::make_unique<ProjectRepository>(databaseManager_);
    taskRepository_ = std::make_unique<TaskRepository>(databaseManager_);
    contentService_ = std::make_unique<ContentService>(*contentRepository_, *projectRepository_, *contentTypeRepository_);
    projectService_ = std::make_unique<ProjectService>(*projectRepository_, *contentTypeRepository_);
    taskService_ = std::make_unique<TaskService>(*taskRepository_, *projectRepository_, *contentRepository_);
    auto *shell = new AppShell(*projectService_, *contentService_, *taskService_, this);
    setCentralWidget(shell);
}

MainWindow::~MainWindow() = default;