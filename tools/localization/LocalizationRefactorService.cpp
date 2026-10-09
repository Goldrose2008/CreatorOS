#include "LocalizationRefactorService.h"
#include "LocalizationSourceScanner.h"
#include "LocalizationTsvStore.h"

#include <algorithm>

#include <QDir>
#include <QFile>
#include <QHash>
#include <QSaveFile>
#include <QSet>
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

bool LocalizationRefactorService::applyRename(const QString &sourceRoot, const QString &localizationPath, LocalizationCatalog &catalog, LocalizationUsageIndex &usageIndex, const QString &oldId, const QString &newId, QString *error) const
{
    const QString normalizedOldId = oldId.trimmed();
    const QString normalizedNewId = newId.trimmed();

    if (!catalog.contains(normalizedOldId))
    {
        if (error)
        {
            *error = QStringLiteral("Localization ID not found: %1").arg(normalizedOldId);
        }
        return false;
    }

    if (catalog.contains(normalizedNewId))
    {
        if (error)
        {
            *error = QStringLiteral("Localization ID already exists: %1").arg(normalizedNewId);
        }
        return false;
    }

    LocalizationRenamePreview preview;

    if (!previewRename(sourceRoot, usageIndex, normalizedOldId, normalizedNewId, preview, error))
    {
        return false;
    }

    QDir sourceDirectory(sourceRoot);

    QHash<QString, QByteArray> originalFiles;
    QHash<QString, QByteArray> updatedFiles;
    QHash<QString, QVector<LocalizationRenameChange>> changesByFile;

    for (const LocalizationRenameChange &change : preview.changes)
    {
        changesByFile[change.filePath].append(change);
    }

    for (auto it = changesByFile.cbegin(); it != changesByFile.cend(); ++it)
    {
        const QString relativePath = it.key();
        const QString absolutePath = sourceDirectory.absoluteFilePath(relativePath);
        QFile file(absolutePath);

        if (!file.open(QIODevice::ReadOnly))
        {
            if (error)
            {
                *error = QStringLiteral("Could not open source file: %1").arg(absolutePath);
            }
            return false;
        }

        const QByteArray originalContent = file.readAll();
        QString updatedContent = QString::fromUtf8(originalContent);
        QVector<LocalizationRenameChange> fileChanges = it.value();

        std::sort(fileChanges.begin(), fileChanges.end(), [](const LocalizationRenameChange &left, const LocalizationRenameChange &right)
            {
                if (left.line != right.line)
                {
                    return left.line > right.line;
                }
                return left.column > right.column;
            });

        for (const LocalizationRenameChange &change : fileChanges)
        {
            qsizetype lineStart = 0;

            for (int currentLine = 1; currentLine < change.line; ++currentLine)
            {
                const qsizetype nextLine = updatedContent.indexOf('\n', lineStart);

                if (nextLine < 0)
                {
                    if (error)
                    {
                        *error = QStringLiteral("Source changed while preparing Rename: %1:%2:%3.").arg(change.filePath).arg(change.line).arg(change.column);
                    }
                    return false;
                }
                lineStart = nextLine + 1;
            }

            const qsizetype position = lineStart + change.column - 1;

            if (position < 0 || position + change.oldId.size() > updatedContent.size() || updatedContent.mid(position, change.oldId.size()) != change.oldId)
            {
                if (error)
                {
                    *error = QStringLiteral("Source changed while preparing Rename: %1:%2:%3.").arg(change.filePath).arg(change.line).arg(change.column);
                }
                return false;
            }
            updatedContent.replace(position, change.oldId.size(), change.newId);
        }
        originalFiles.insert(relativePath, originalContent);
        updatedFiles.insert(relativePath, updatedContent.toUtf8());
    }

    QStringList committedFiles;

    const auto writeAtomic = [](const QString &path, const QByteArray &content, QString *writeError) -> bool
        {
            QSaveFile file(path);

            if (!file.open(QIODevice::WriteOnly))
            {
                if (writeError)
                {
                    *writeError = QStringLiteral("Could not open file for atomic write: %1").arg(path);
                }
                return false;
            }

            const qint64 written = file.write(content);

            if (written != content.size())
            {
                if (writeError)
                {
                    *writeError = QStringLiteral("Could not write complete file: %1").arg(path);
                }
                file.cancelWriting();
                return false;
            }

            if (!file.commit())
            {
                if (writeError)
                {
                    *writeError = QStringLiteral("Could not commit file: %1").arg(path);
                }
                return false;
            }
            return true;
        };

    const auto rollback = [&]()
        {
            QString rollbackError;

            for (const QString &relativePath : committedFiles)
            {
                const QString absolutePath = sourceDirectory.absoluteFilePath(relativePath);

                if (!writeAtomic(absolutePath, originalFiles.value(relativePath), &rollbackError))
                {
                    return rollbackError;
                }
            }
            return QString();
        };

    for (auto it = updatedFiles.cbegin(); it != updatedFiles.cend(); ++it)
    {
        const QString absolutePath = sourceDirectory.absoluteFilePath(it.key());
        QString writeError;

        if (!writeAtomic(absolutePath, it.value(), &writeError))
        {
            const QString rollbackError = rollback();

            if (error)
            {
                if (rollbackError.isEmpty())
                {
                    *error = writeError;
                }
                else
                {
                    *error = QStringLiteral("%1\nRollback failed: %2").arg(writeError, rollbackError);
                }
            }
            return false;
        }
        committedFiles.append(it.key());
    }

    LocalizationSourceScanner scanner;
    LocalizationUsageIndex refreshedUsageIndex;
    QString scanError;

    if (!scanner.scan(sourceRoot, refreshedUsageIndex, &scanError))
    {
        const QString rollbackError = rollback();

        if (error)
        {
            if (rollbackError.isEmpty())
            {
                *error = QStringLiteral("Rename was not completed because the post-change source scan failed: %1").arg(scanError);
            }
            else
            {
                *error = QStringLiteral("Post-change source scan failed: %1\n Rollback failed: %2").arg(scanError, rollbackError);
            }
        }
        return false;
    }

    const LocalizationCatalog originalCatalog = catalog;

    if (!catalog.renameId(normalizedOldId, normalizedNewId, error))
    {
        const QString rollbackError = rollback();

        if (error && !rollbackError.isEmpty())
        {
            *error += QStringLiteral("\nRollback failed: %1").arg(rollbackError);
        }
        return false;
    }

    QString saveError;

    if (!LocalizationTsvStore::save(localizationPath, catalog, &saveError))
    {
        catalog = originalCatalog;
        const QString rollbackError = rollback();

        if (error)
        {
            if (rollbackError.isEmpty())
            {
                *error = QStringLiteral("Could not save localization.tsv: %1").arg(saveError);
            }
            else
            {
                *error = QStringLiteral("Could not save localization.tsv: %1\n Rollback failed: %2").arg(saveError, rollbackError);
            }
        }
        return false;
    }
    usageIndex = refreshedUsageIndex;
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