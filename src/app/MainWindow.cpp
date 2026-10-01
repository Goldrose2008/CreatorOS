#include "MainWindow.h"
//#include "../ui/shell/WebUiHost.h"
#include <QWidget>   // временно

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("CreatorOS");
    resize(1280, 800);

    //auto *webUi = new WebUiHost(this);
    //setCentralWidget(webUi);
}