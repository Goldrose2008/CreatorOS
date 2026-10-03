#include "MainWindow.h"

#include <QMessageBox>

#include "AppInfo.h"
#include "../ui/shell/AppShell.h"
#include "../application/projects/ProjectService.h"
#include "../infrastructure/database/ProjectRepository.h"

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

    projectRepository_ = std::make_unique<ProjectRepository>(databaseManager_);

    projectService_ = std::make_unique<ProjectService>(*projectRepository_);

    auto *shell = new AppShell(*projectService_, this);
    setCentralWidget(shell);
}

MainWindow::~MainWindow() = default;