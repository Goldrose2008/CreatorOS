#pragma once

#include "LocalizationUsageIndex.h"

#include <QString>

class LocalizationSourceScanner final
{
public:
    bool scan(const QString &sourceRoot, LocalizationUsageIndex &index, QString *error = nullptr) const;

private:
    bool scanFile(const QString &filePath, const QString &sourceRoot, LocalizationUsageIndex &index, QString *error) const;
};