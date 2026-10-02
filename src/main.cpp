#include <QApplication>
#include <QWebEngineUrlScheme>

#include "app/MainWindow.h"
#include "app/AppInfo.h"

int main(int argc, char *argv[])
{
    QWebEngineUrlScheme qrcScheme(QByteArrayLiteral("qrc"));

    qrcScheme.setFlags(
        QWebEngineUrlScheme::SecureScheme
        | QWebEngineUrlScheme::LocalAccessAllowed
        | QWebEngineUrlScheme::CorsEnabled
        | QWebEngineUrlScheme::ViewSourceAllowed
        | QWebEngineUrlScheme::FetchApiAllowed
    );

    QWebEngineUrlScheme::registerScheme(qrcScheme);

    QApplication application(argc, argv);

    QApplication::setApplicationName(AppInfo::Name);
    QApplication::setApplicationVersion(AppInfo::Version);

    MainWindow window;
    window.show();

    return application.exec();
}