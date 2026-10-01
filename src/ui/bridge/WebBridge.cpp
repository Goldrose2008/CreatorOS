#include "WebBridge.h"

#include <QCoreApplication>

WebBridge::WebBridge(QObject *parent)
    : QObject(parent)
{
}

QString WebBridge::applicationName() const
{
    return QCoreApplication::applicationName();
}

QString WebBridge::applicationVersion() const
{
    return QCoreApplication::applicationVersion();
}