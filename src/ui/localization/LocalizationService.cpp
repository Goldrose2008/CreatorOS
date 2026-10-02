#include "LocalizationService.h"

#include <QFile>
#include <QStringList>
#include <QStringConverter>
#include <QTextStream>

LocalizationService::LocalizationService(QObject *parent) : QObject(parent)
{
}

bool LocalizationService::load(const QString &resourcePath)
{
    QFile file(resourcePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);

    columnIndexes_.clear();
    entries_.clear();

    const QString headerLine = stream.readLine();
    const QStringList headers = headerLine.split('\t', Qt::KeepEmptyParts);

    for (int index = 0; index < headers.size(); ++index)
    {
        columnIndexes_.insert(headers.at(index).trimmed(), index);
    }

    if (!columnIndexes_.contains("id") || !columnIndexes_.contains("ru"))
    {
        return false;
    }

    while (!stream.atEnd())
    {
        const QString line = stream.readLine();

        if (line.trimmed().isEmpty())
        {
            continue;
        }

        const QStringList values = line.split('\t', Qt::KeepEmptyParts);
        const int idIndex = columnIndexes_.value("id");

        if (values.size() <= idIndex)
        {
            continue;
        }

        const QString id = values.at(idIndex).trimmed();

        if (id.isEmpty())
        {
            continue;
        }

        QHash<QString, QString> localizedValues;

        for (auto it = columnIndexes_.cbegin(); it != columnIndexes_.cend(); ++it)
        {
            const int index = it.value();

            if (index >= 0 && index < values.size())
            {
                localizedValues.insert(it.key(), values.at(index));
            }
        }

        entries_.insert(id, localizedValues);
    }

    return true;
}

QString LocalizationService::text(const QString &id) const
{
    const auto entry = entries_.value(id);

    if (entry.contains(currentLocale_))
    {
        return entry.value(currentLocale_);
    }

    return entry.value("ru", id);
}

QString LocalizationService::locale() const
{
    return currentLocale_;
}

bool LocalizationService::setLocale(const QString &locale)
{
    if (!columnIndexes_.contains(locale))
    {
        return false;
    }

    currentLocale_ = locale;
    return true;
}