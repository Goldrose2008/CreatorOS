#include <QApplication>
#include <QWebEngineUrlScheme>

#include "app/MainWindow.h"

int main(int argc, char *argv[])
{
    QWebEngineUrlScheme qrcScheme(QByteArrayLiteral("qrc"));

    qrcScheme.setFlags(
        QWebEngineUrlScheme::SecureScheme
        | QWebEngineUrlScheme::LocalAccessAllowed
        | QWebEngineUrlScheme::CorsEnabled
        | QWebEngineUrlScheme::ViewSourceAllowed
    );

    QWebEngineUrlScheme::registerScheme(qrcScheme);

    QApplication application(argc, argv);

    QApplication::setApplicationName("CreatorOS");
    QApplication::setApplicationVersion("0.1.0");

    MainWindow window;
    window.show();

    return application.exec();
}