#include "LocalizationRefactorService.h"

#include <QDir>
#include <QFile>
#include <QStringConverter>
#include <QTextStream>

bool LocalizationRefactorService::previewRename(const QString &sourceRoot, const LocalizationUsageIndex &usageIndex, const QString &oldId, const QString &newId, LocalizationRenamePreview &preview, QString *error) const
{
    preview = LocalizationRenamePreview{};

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

    if (normalizedOldId == normalizedNewId)
    {
        if (error)
        {
            *error = QStringLiteral("New localization ID must differ from the old ID.");
        }

        return false;
    }

    const QVector<LocalizationUsage> usages = usageIndex.usagesFor(normalizedOldId);

    for (const LocalizationUsage &usage : usages)
    {
        QString sourceLine;

        if (!readSourceLine(sourceRoot, usage, sourceLine, error))
        {
            return false;
        }

        const int oldIdStart = usage.column - 1;

        if (oldIdStart < 0 || oldIdStart + normalizedOldId.size() > sourceLine.size() || sourceLine.mid(oldIdStart, normalizedOldId.size()) != normalizedOldId)
        {
            if (error)
            {
                *error = QStringLiteral("Source changed since scan: %1:%2:%3.").arg(usage.filePath).arg(usage.line).arg(usage.column);
            }

            return false;
        }

        QString updatedLine = sourceLine;
        updatedLine.replace(oldIdStart, normalizedOldId.size(), normalizedNewId);
        preview.changes.append(LocalizationRenameChange{usage.filePath, usage.line, usage.column, normalizedOldId, normalizedNewId, sourceLine, updatedLine});
    }

    preview.dynamicReferences = usageIndex.dynamicReferences();
    return true;
}

bool LocalizationRefactorService::readSourceLine(const QString &sourceRoot, const LocalizationUsage &usage, QString &line, QString *error) const
{
    QDir sourceDirectory(sourceRoot);
    const QString absolutePath = sourceDirectory.absoluteFilePath(usage.filePath);
    QFile file(absolutePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        if (error)
        {
            *error = QStringLiteral("Could not open source file: %1").arg(absolutePath);
        }

        return false;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);

    for (int currentLine = 1; currentLine <= usage.line; ++currentLine)
    {
        if (stream.atEnd())
        {
            if (error)
            {
                *error = QStringLiteral("Source changed since scan: %1:%2:%3.").arg(usage.filePath).arg(usage.line).arg(usage.column);
            }

            return false;
        }

        line = stream.readLine();
    }

    return true;
}