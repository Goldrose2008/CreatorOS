#include "LocalizationValidator.h"

#include <QHash>
#include <QRegularExpression>
#include <QStringList>

#include <algorithm>

namespace
{
    QStringList extractPlaceholders(const QString &value)
    {
        static const QRegularExpression pattern(QStringLiteral("%L?(?:[1-9][0-9]*|n)"));

        QStringList result;
        auto iterator = pattern.globalMatch(value);

        while (iterator.hasNext())
        {
            QString placeholder = iterator.next().captured(0);

            // %L1 и %1 обозначают один и тот же аргумент.
            // Аналогично %Ln и %n.
            if (placeholder.startsWith(QStringLiteral("%L")))
            {
                placeholder.remove(1, 1);
            }
            result.append(placeholder);
        }

        // Сравниваем наборы независимо от порядка
        // аргументов в переводе, сохраняя количество повторений.
        std::sort(result.begin(), result.end());
        return result;
    }
}

QVector<LocalizationValidationIssue>LocalizationValidator::validate( const LocalizationCatalog &catalog, const LocalizationUsageIndex &usageIndex) const
{
    QVector<LocalizationValidationIssue> issues;

    const auto appendIssue = [&issues](
            LocalizationValidationSeverity severity,
            LocalizationValidationType type,
            const QString &id,
            const QString &locale,
            const QString &filePath,
            int line,
            int column,
            const QString &message)
        {
            issues.append(LocalizationValidationIssue{severity, type, id, locale, filePath, line, column, message});
        };

    // Запоминаем первую запись с каждым переводом
    // в пределах конкретного языка.
    QHash<QString, QHash<QString, QString>> seenTranslations;
    const QStringList locales = catalog.locales();

    for (const QString &id : catalog.ids())
    {
        const LocalizationEntry *entry = catalog.find(id);
        if (!entry){continue;}

        bool hasReferencePlaceholders = false;
        QString referenceLocale;
        QStringList referencePlaceholders;

        for (const QString &locale : locales)
        {
            const QString value = entry->translations.value(locale);

            if (value.trimmed().isEmpty())
            {
                appendIssue(
                    LocalizationValidationSeverity::Error,
                    LocalizationValidationType::EmptyTranslation, id, locale, QString(), 0, 0, QStringLiteral("Перевод для языка '%1' пуст.").arg(locale));
            }

            const QStringList currentPlaceholders = extractPlaceholders(value);

            if (!hasReferencePlaceholders)
            {
                referencePlaceholders = currentPlaceholders;
                referenceLocale = locale;
                hasReferencePlaceholders = true;
            }
            else if (currentPlaceholders != referencePlaceholders)
            {
                appendIssue(
                    LocalizationValidationSeverity::Error,
                    LocalizationValidationType::PlaceholderMismatch, id, locale, QString(), 0, 0, QStringLiteral("Набор подстановок отличается от языка '%1'.").arg(referenceLocale));
            }

            const QString normalizedValue = value.simplified().toCaseFolded();

            // Пустые значения не участвуют в поиске дублей.
            if (normalizedValue.isEmpty()){continue;}

            QHash<QString, QString> &translationsForLocale = seenTranslations[locale];

            if (translationsForLocale.contains(normalizedValue))
            {
                const QString firstId = translationsForLocale.value(normalizedValue);

                if (firstId != id)
                {
                    appendIssue(
                        LocalizationValidationSeverity::Warning,
                        LocalizationValidationType::DuplicateTranslation, id, locale, QString(), 0, 0, QStringLiteral("Перевод совпадает с записью '%1' для языка '%2'.").arg(firstId, locale));
                }
            }
            else
            {
                translationsForLocale.insert(normalizedValue, id);
            }
        }

        if (usageIndex.usageCount(id) == 0)
        {
            appendIssue(
                LocalizationValidationSeverity::Warning,
                LocalizationValidationType::UnusedId, id, QString(), QString(), 0, 0, QStringLiteral("Для ID не найдено статических использований."));
        }
    }

    // Статические обращения к ID, отсутствующим в каталоге.
    for (const QString &id : usageIndex.ids())
    {
        if (catalog.contains(id)){continue;}
        const QVector<LocalizationUsage> usages = usageIndex.usagesFor(id);

        for (const LocalizationUsage &usage : usages)
        {
            appendIssue(
                LocalizationValidationSeverity::Error,
                LocalizationValidationType::MissingId, id, QString(), usage.filePath, usage.line, usage.column, QStringLiteral("Исходный код использует ID, отсутствующий в каталоге."));
        }
    }

    // Динамические ссылки не сопоставляются с конкретными ID.
    // Поэтому они остаются предупреждениями, а не MissingId.
    for (const LocalizationDynamicReference &reference : usageIndex.dynamicReferences())
    {
        appendIssue(LocalizationValidationSeverity::Warning, LocalizationValidationType::DynamicReference, QString(), QString(), reference.filePath, reference.line, reference.column, QStringLiteral("Динамическую localization-ссылку не удалось сопоставить с конкретным ID."));
    }
    return issues;
}