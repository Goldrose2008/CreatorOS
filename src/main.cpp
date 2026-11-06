#include <QApplication>
#include <QMessageBox>

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
    QString initializationError;

    if (!window.initialize(&initializationError))
    {
        QMessageBox::critical(nullptr, QString::fromLatin1(AppInfo::Name), initializationError);
        return 1;
    }

    window.show();

    return application.exec();
}