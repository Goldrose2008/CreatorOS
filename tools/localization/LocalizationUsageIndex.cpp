#include "LocalizationUsageIndex.h"

#include <algorithm>

void LocalizationUsageIndex::clear()
{
    usages_.clear();
    dynamicReferences_.clear();
}

void LocalizationUsageIndex::addStaticUsage(const QString &id, const LocalizationUsage &usage)
{
    const QString normalizedId = id.trimmed();
    if (normalizedId.isEmpty()){return;}
    usages_[normalizedId].append(usage);
}

void LocalizationUsageIndex::addDynamicReference(const LocalizationDynamicReference &reference)
{
    dynamicReferences_.append(reference);
}

QStringList LocalizationUsageIndex::ids() const
{
    QStringList result = usages_.keys();
    std::sort(result.begin(), result.end());
    return result;
}

int LocalizationUsageIndex::usageCount(const QString &id) const
{
    return usages_.value(id).size();
}

QVector<LocalizationUsage> LocalizationUsageIndex::usagesFor(const QString &id) const
{
    return usages_.value(id);
}

const QVector<LocalizationDynamicReference> &
LocalizationUsageIndex::dynamicReferences() const
{
    return dynamicReferences_;
}

int LocalizationUsageIndex::staticUsageCount() const
{
    int result = 0;

    for (const auto &usages : usages_)
    {
        result += usages.size();
    }

    return result;
}