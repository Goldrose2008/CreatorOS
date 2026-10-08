#pragma once

#include "LocalizationCatalog.h"

#include <QString>

class LocalizationTsvStore final
{
public:
    static bool load(const QString &filePath, LocalizationCatalog &catalog, QString *error = nullptr);
    static bool save(const QString &filePath, const LocalizationCatalog &catalog, QString *error = nullptr);
};