#pragma once

#include "LocalizationUsageIndex.h"
#include "LocalizationCatalog.h"

#include <QString>
#include <QVector>

struct LocalizationRenameChange
{
    QString filePath;
    int line = 0;
    int column = 0;
    QString oldId;
    QString newId;
    QString beforeLine;
    QString afterLine;
};

struct LocalizationRenamePreview
{
    QVector<LocalizationRenameChange> changes;
    QVector<LocalizationDynamicReference> dynamicReferences;
};

class LocalizationRefactorService final
{
public:
    bool previewRename(const QString &sourceRoot, const LocalizationUsageIndex &usageIndex, const QString &oldId, const QString &newId, LocalizationRenamePreview &preview, QString *error = nullptr) const;
    bool applyRename(const QString &sourceRoot, const QString &localizationPath, LocalizationCatalog &catalog, LocalizationUsageIndex &usageIndex, const QString &oldId, const QString &newId, QString *error = nullptr) const;

private:
    bool readSourceLine(const QString &sourceRoot, const LocalizationUsage &usage, QString &line, QString *error) const;
};