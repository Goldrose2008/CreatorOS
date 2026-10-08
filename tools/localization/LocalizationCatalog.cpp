#include "LocalizationCatalog.h"

#include <algorithm>

void LocalizationCatalog::setLocales(const QStringList &locales)
{
    locales_ = locales;
}

const QStringList &LocalizationCatalog::locales() const
{
    return locales_;
}

QStringList LocalizationCatalog::ids() const
{
    QStringList result = entries_.keys();
    std::sort(result.begin(), result.end());
    return result;
}

int LocalizationCatalog::size() const
{
    return entries_.size();
}

bool LocalizationCatalog::contains(const QString &id) const
{
    return entries_.contains(id);
}

const LocalizationEntry *LocalizationCatalog::find(const QString &id) const
{
    const auto it = entries_.constFind(id);
    if (it == entries_.constEnd()){return nullptr;}
    return &it.value();
}

bool LocalizationCatalog::addEntry(const LocalizationEntry &entry, QString *error)
{
    const QString id = entry.id.trimmed();

    if (id.isEmpty())
    {
        if (error)
        {
            *error = QStringLiteral("Localization ID cannot be empty.");
        }

        return false;
    }

    if (entries_.contains(id))
    {
        if (error)
        {
            *error = QStringLiteral("Localization ID already exists: %1").arg(id);
        }

        return false;
    }

    LocalizationEntry normalizedEntry = entry;
    normalizedEntry.id = id;
    entries_.insert(id, normalizedEntry);
    
    return true;
}

bool LocalizationCatalog::updateTranslation(const QString &id, const QString &locale, const QString &value, QString *error)
{
    const QString normalizedId = id.trimmed();
    const QString normalizedLocale = locale.trimmed();
    auto it = entries_.find(normalizedId);

    if (it == entries_.end())
    {
        if (error)
        {
            *error = QStringLiteral("Localization ID not found: %1").arg(normalizedId);
        }

        return false;
    }

    if (!locales_.contains(normalizedLocale))
    {
        if (error)
        {
            *error = QStringLiteral("Unknown locale: %1").arg(normalizedLocale);
        }

        return false;
    }

    it.value().translations.insert(normalizedLocale, value);
    return true;
}

bool LocalizationCatalog::removeEntry(const QString &id, QString *error)
{
    const QString normalizedId = id.trimmed();

    if (!entries_.contains(normalizedId))
    {
        if (error)
        {
            *error = QStringLiteral("Localization ID not found: %1").arg(normalizedId);
        }

        return false;
    }

    entries_.remove(normalizedId);
    return true;
}

bool LocalizationCatalog::renameId(const QString &oldId, const QString &newId, QString *error)
{
    const QString normalizedOldId = oldId.trimmed();
    const QString normalizedNewId = newId.trimmed();

    if (normalizedOldId.isEmpty() || normalizedNewId.isEmpty())
    {
        if (error)
        {
            *error = QStringLiteral("Localization IDs cannot be empty.");
        }

        return false;
    }

    if (!entries_.contains(normalizedOldId))
    {
        if (error)
        {
            *error = QStringLiteral("Localization ID not found: %1").arg(normalizedOldId);
        }

        return false;
    }

    if (normalizedOldId == normalizedNewId){return true;}

    if (entries_.contains(normalizedNewId))
    {
        if (error)
        {
            *error = QStringLiteral("Localization ID already exists: %1").arg(normalizedNewId);
        }

        return false;
    }

    LocalizationEntry entry = entries_.value(normalizedOldId);
    entry.id = normalizedNewId;

    entries_.remove(normalizedOldId);
    entries_.insert(normalizedNewId, entry);

    return true;
}