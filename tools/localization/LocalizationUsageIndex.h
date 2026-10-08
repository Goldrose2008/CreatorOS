#pragma once

#include "LocalizationUsage.h"

#include <QHash>
#include <QStringList>
#include <QVector>

class LocalizationUsageIndex final
{
public:
    void clear();
    void addStaticUsage(const QString &id, const LocalizationUsage &usage);
    void addDynamicReference(const LocalizationDynamicReference &reference);
    QStringList ids() const;
    int usageCount(const QString &id) const;
    QVector<LocalizationUsage> usagesFor(const QString &id) const;
    const QVector<LocalizationDynamicReference> &dynamicReferences() const;
    int staticUsageCount() const;

private:
    QHash<QString, QVector<LocalizationUsage>> usages_;
    QVector<LocalizationDynamicReference> dynamicReferences_;
};