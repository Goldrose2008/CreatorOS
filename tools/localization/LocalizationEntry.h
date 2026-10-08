#pragma once

#include <QMap>
#include <QString>

struct LocalizationEntry
{
    QString id;
    QMap<QString, QString> translations;
};