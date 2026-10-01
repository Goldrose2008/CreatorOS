#pragma once

#include <QObject>
#include <QString>

class WebBridge final : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString applicationName
               READ applicationName
               CONSTANT)

    Q_PROPERTY(QString applicationVersion
               READ applicationVersion
               CONSTANT)

public:
    explicit WebBridge(QObject *parent = nullptr);

    QString applicationName() const;
    QString applicationVersion() const;
};