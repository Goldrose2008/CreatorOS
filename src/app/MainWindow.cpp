#include "MainWindow.h"

#include "AppInfo.h"
#include "../ui/shell/AppShell.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle(QString::fromLatin1(AppInfo::Name));
    resize(1280, 800);
    auto *shell = new AppShell(this);
    setCentralWidget(shell);
}