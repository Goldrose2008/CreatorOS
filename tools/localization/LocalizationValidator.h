#pragma once

#include "LocalizationCatalog.h"
#include "LocalizationUsageIndex.h"
#include "LocalizationValidationIssue.h"

#include <QVector>

class LocalizationValidator final
{
public:
    QVector<LocalizationValidationIssue> validate(const LocalizationCatalog &catalog, const LocalizationUsageIndex &usageIndex) const;
};