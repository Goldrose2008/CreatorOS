#include "LocalizationSourceScanner.h"

#include <QSet>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QRegularExpression>
#include <QStringConverter>
#include <QTextStream>

namespace
{
    LocalizationUsage sourceLocation(const QString &content, qsizetype position, const QString &filePath, const QString &sourceRoot)
    {
        const qsizetype lineStart = content.lastIndexOf('\n', position - 1);
        const int line = content.left(position).count('\n') + 1;
        const int column = static_cast<int>(position - (lineStart < 0 ? -1 : lineStart));

        QDir root(sourceRoot);

        return LocalizationUsage{
            root.relativeFilePath(filePath).replace('\\', '/'),
            line,
            column
        };
    }

    LocalizationDynamicReference dynamicLocation(const QString &content, qsizetype position, const QString &filePath, const QString &sourceRoot)
    {
        const qsizetype lineStart = content.lastIndexOf('\n', position - 1);
        const int line = content.left(position).count('\n') + 1;
        const int column = static_cast<int>(position - (lineStart < 0 ? -1 : lineStart));

        QDir root(sourceRoot);

        return LocalizationDynamicReference{
            root.relativeFilePath(filePath).replace('\\', '/'),
            line,
            column
        };
    }
}

bool LocalizationSourceScanner::scan(const QString &sourceRoot, LocalizationUsageIndex &index, QString *error) const
{
    QDir sourceDirectory(sourceRoot);

    if (!sourceDirectory.exists())
    {
        if (error)
        {
            *error = QStringLiteral("Source directory does not exist: %1").arg(sourceRoot);
        }

        return false;
    }

    index.clear();

    QDirIterator iterator(
        sourceDirectory.absolutePath(),
        QStringList{
            QStringLiteral("*.cpp"),
            QStringLiteral("*.h")
        },
        QDir::Files,
        QDirIterator::Subdirectories);

    while (iterator.hasNext())
    {
        const QString filePath = iterator.next();

        if (!scanFile(filePath, sourceDirectory.absolutePath(), index, error))
        {
            return false;
        }
    }

    return true;
}

bool LocalizationSourceScanner::scanFile(const QString &filePath, const QString &sourceRoot, LocalizationUsageIndex &index, QString *error) const
{
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        if (error)
        {
            *error = QStringLiteral("Could not open source file: %1").arg(filePath);
        }

        return false;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);

    const QString content = stream.readAll();

    const QRegularExpression staticPattern(
    QStringLiteral(R"LOCALIZATION(\blocalization[A-Za-z0-9_]*\s*(?:->|\.)\s*text\s*\(\s*QStringLiteral\s*\(\s*"([^"]+)"\s*\)\s*\))LOCALIZATION"), QRegularExpression::CaseInsensitiveOption);
    const QRegularExpression callPattern(QStringLiteral(R"(\blocalization[A-Za-z0-9_]*\s*(?:->|\.)\s*text\s*\()"), QRegularExpression::CaseInsensitiveOption);

    QSet<qsizetype> staticReferencePositions;

    auto staticIterator = staticPattern.globalMatch(content);

    while (staticIterator.hasNext())
    {
        const QRegularExpressionMatch match = staticIterator.next();

        const QString id = match.captured(1);
        const qsizetype idPosition = match.capturedStart(1);

        staticReferencePositions.insert(match.capturedStart());

        index.addStaticUsage(id, sourceLocation(content, idPosition, filePath, sourceRoot));
    }

    auto callIterator = callPattern.globalMatch(content);

    while (callIterator.hasNext())
    {
        const QRegularExpressionMatch match = callIterator.next();
        const qsizetype callPosition = match.capturedStart();

        if (staticReferencePositions.contains(callPosition))
        {
            continue;
        }

        index.addDynamicReference(dynamicLocation(content, callPosition, filePath, sourceRoot));
    }

    return true;
}