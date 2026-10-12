#include "LocalizationCatalog.h"

#include <algorithm>
#include <QRegularExpression>

void LocalizationCatalog::setLocales(const QStringList &locales)
{
    locales_ = locales;
}

bool LocalizationCatalog::addLocale(const QString &locale, const QString &sourceLocale, QString *error)
{
    const QString normalizedLocale = locale.trimmed();
    const QString normalizedSourceLocale = sourceLocale.trimmed();
    static const QRegularExpression localePattern(QStringLiteral("^[a-z]{2,3}(?:_[A-Z]{2})?$"));

    if (!localePattern.match(normalizedLocale).hasMatch())
    {
        if (error)
        {
            *error = QStringLiteral("Код языка должен содержать код языка в нижнем регистре и необязательный код региона в верхнем регистре, например 'de' или 'pt_BR'.");
        }
        return false;
    }

    if (locales_.contains(normalizedLocale))
    {
        if (error)
        {
            *error = QStringLiteral("Язык уже существует: %1").arg(normalizedLocale);
        }

        return false;
    }

    if (!locales_.contains(normalizedSourceLocale))
    {
        if (error)
        {
            *error = QStringLiteral("Исходный язык не существует: %1").arg(normalizedSourceLocale);
        }

        return false;
    }

    // Сначала проверяем все значения источника.
    // До завершения проверки каталог не изменяется.
    for (auto it = entries_.cbegin(); it != entries_.cend(); ++it)
    {
        const QString sourceValue = it.value().translations.value(normalizedSourceLocale);

        if (sourceValue.trimmed().isEmpty())
        {
            if (error)
            {
                *error = QStringLiteral("Исходный перевод для ID '%1' на языке '%2' пуст.").arg(it.key(), normalizedSourceLocale);
            }

            return false;
        }
    }

    locales_.append(normalizedLocale);

    for (auto it = entries_.begin(); it != entries_.end(); ++it)
    {
        const QString sourceValue = it.value().translations.value(normalizedSourceLocale);

        it.value().translations.insert(normalizedLocale, sourceValue);
    }

    if (error){error->clear();}
    return true;
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
            *error = QStringLiteral("ID локализации не может быть пустым.");
        }

        return false;
    }

    if (entries_.contains(id))
    {
        if (error)
        {
            *error = QStringLiteral("ID локализации уже существует: %1").arg(id);
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
            *error = QStringLiteral("ID локализации не найден: %1").arg(normalizedId);
        }

        return false;
    }

    if (!locales_.contains(normalizedLocale))
    {
        if (error)
        {
            *error = QStringLiteral("Неизвестный язык: %1").arg(normalizedLocale);
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
            *error = QStringLiteral("ID локализации не найден: %1").arg(normalizedId);
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
            *error = QStringLiteral("ID локализации не может быть пустым.");
        }

        return false;
    }

    if (!entries_.contains(normalizedOldId))
    {
        if (error)
        {
            *error = QStringLiteral("ID локализации не найден: %1").arg(normalizedOldId);
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