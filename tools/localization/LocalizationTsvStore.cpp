#include "LocalizationTsvStore.h"

#include <QFile>
#include <QSaveFile>
#include <QStringConverter>
#include <QStringList>
#include <QTextStream>

bool LocalizationTsvStore::load(const QString &filePath, LocalizationCatalog &catalog, QString *error)
{
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        if (error)
        {
            *error = QStringLiteral("Could not open localization file: %1").arg(filePath);
        }

        return false;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);

    const QString headerLine = stream.readLine();

    if (headerLine.isNull())
    {
        if (error)
        {
            *error = QStringLiteral("Localization file is empty.");
        }

        return false;
    }

    QStringList headers = headerLine.split('\t', Qt::KeepEmptyParts);

    for (QString &header : headers)
    {
        header = header.trimmed();

        if (!header.isEmpty() && header.at(0) == QChar::ByteOrderMark)
        {
            header.remove(0, 1);
        }
    }

    const int idIndex = headers.indexOf(QStringLiteral("id"));

    if (idIndex < 0)
    {
        if (error)
        {
            *error = QStringLiteral("Localization file must contain an 'id' column.");
        }

        return false;
    }

    if (!headers.contains(QStringLiteral("ru")))
    {
        if (error)
        {
            *error = QStringLiteral("Localization file must contain a 'ru' column.");
        }

        return false;
    }

    QStringList locales;

    for (int index = 0; index < headers.size(); ++index)
    {
        if (index == idIndex)
        {
            continue;
        }

        const QString locale = headers.at(index);

        if (locale.isEmpty())
        {
            if (error)
            {
                *error = QStringLiteral("Localization header contains an empty column.");
            }

            return false;
        }

        if (locales.contains(locale))
        {
            if (error)
            {
                *error = QStringLiteral("Duplicate localization column: %1").arg(locale);
            }

            return false;
        }

        locales.append(locale);
    }

    LocalizationCatalog loadedCatalog;
    loadedCatalog.setLocales(locales);

    int lineNumber = 1;

    while (!stream.atEnd())
    {
        ++lineNumber;
        const QString line = stream.readLine();

        if (line.trimmed().isEmpty())
        {
            continue;
        }

        const QStringList values = line.split('\t', Qt::KeepEmptyParts);

        if (values.size() != headers.size())
        {
            if (error)
            {
                *error = QStringLiteral(
                             "Invalid column count at line %1: expected %2, got %3.")
                             .arg(lineNumber)
                             .arg(headers.size())
                             .arg(values.size());
            }

            return false;
        }

        const QString id = values.at(idIndex).trimmed();

        if (id.isEmpty())
        {
            if (error)
            {
                *error = QStringLiteral("Localization ID is empty at line %1.").arg(lineNumber);
            }

            return false;
        }

        LocalizationEntry entry;
        entry.id = id;

        for (int index = 0; index < headers.size(); ++index)
        {
            if (index == idIndex)
            {
                continue;
            }

            entry.translations.insert(headers.at(index), values.at(index));
        }

        QString addError;

        if (!loadedCatalog.addEntry(entry, &addError))
        {
            if (error)
            {
                *error = QStringLiteral("Line %1: %2").arg(lineNumber).arg(addError);
            }

            return false;
        }
    }

    catalog = loadedCatalog;
    return true;
}

bool LocalizationTsvStore::save(const QString &filePath, const LocalizationCatalog &catalog, QString *error)
{
    QSaveFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        if (error)
        {
            *error = QStringLiteral("Could not open localization file for writing: %1").arg(filePath);
        }

        return false;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);

    QStringList header;
    header.append(QStringLiteral("id"));
    header.append(catalog.locales());
    stream << header.join('\t') << '\n';
    const QStringList ids = catalog.ids();

    for (const QString &id : ids)
    {
        const LocalizationEntry *entry = catalog.find(id);

        if (!entry)
        {
            if (error)
            {
                *error = QStringLiteral("Internal error: entry disappeared while saving: %1").arg(id);
            }

            return false;
        }

        QStringList values;
        values.append(id);

        for (const QString &locale : catalog.locales())
        {
            values.append(entry->translations.value(locale));
        }

        stream << values.join('\t') << '\n';
    }

    if (!file.commit())
    {
        if (error)
        {
            *error = QStringLiteral("Could not commit localization file: %1").arg(filePath);
        }

        return false;
    }

    return true;
}