#include <QApplication>

#include "app/MainWindow.h"
#include "app/AppInfo.h"
#include "ui/style/CreatorStyle.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    application.setStyle(new CreatorStyle());

    QApplication::setApplicationName(AppInfo::Name);
    QApplication::setApplicationVersion(AppInfo::Version);

    MainWindow window;
    window.show();

    return application.exec();
}