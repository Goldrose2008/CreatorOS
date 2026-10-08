#pragma once

#include "LocalizationEntry.h"

#include <QStringList>
#include <QHash>

class LocalizationCatalog final
{
public:
    void setLocales(const QStringList &locales);

    const QStringList &locales() const;
    QStringList ids() const;

    int size() const;
    bool contains(const QString &id) const;

    const LocalizationEntry *find(const QString &id) const;

    bool addEntry(const LocalizationEntry &entry, QString *error = nullptr);
    bool updateTranslation(const QString &id, const QString &locale, const QString &value, QString *error = nullptr);
    bool removeEntry(const QString &id, QString *error = nullptr);
    bool renameId(const QString &oldId, const QString &newId, QString *error = nullptr);

private:
    QStringList locales_;
    QHash<QString, LocalizationEntry> entries_;
};