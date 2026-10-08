#pragma once

#include <QString>

struct LocalizationUsage
{
    QString filePath;
    int line = 0;
    int column = 0;
};

struct LocalizationDynamicReference
{
    QString filePath;
    int line = 0;
    int column = 0;
};