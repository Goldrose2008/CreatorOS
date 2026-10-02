#pragma once

#include <QString>

class QWidget;

struct CurrentPage final
{
    QString route;
    QWidget *widget = nullptr;
};