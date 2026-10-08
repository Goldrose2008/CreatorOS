#include "LocalizationSourceScanner.h"
#include "LocalizationTsvStore.h"

#include <QCoreApplication>
#include <QDebug>
#include <QSet>
#include <QStringList>

int main(int argc, char *argv[])
{
    QCoreApplication application(argc, argv);

    const QStringList arguments = application.arguments();

    if (arguments.size() != 3)
    {
        qCritical()
            << "Usage: CreatorOSLocalization "
               "<path-to-localization.tsv> "
               "<source-root>";

        return 1;
    }

    const QString localizationPath = arguments.at(1);
    const QString sourceRoot = arguments.at(2);

    LocalizationCatalog catalog;
    QString error;

    if (!LocalizationTsvStore::load(localizationPath, catalog, &error))
    {
        qCritical() << error;
        return 1;
    }

    LocalizationUsageIndex usageIndex;
    LocalizationSourceScanner scanner;

    if (!scanner.scan(sourceRoot, usageIndex, &error))
    {
        qCritical() << error;
        return 1;
    }

    qInfo() << "Localization catalog loaded successfully.";
    qInfo() << "Entries:" << catalog.size();
    qInfo() << "Locales:" << catalog.locales();
    qInfo() << "Static references:" << usageIndex.staticUsageCount();
    qInfo() << "Unique referenced IDs:" << usageIndex.ids().size();
    qInfo() << "Dynamic references:" << usageIndex.dynamicReferences().size();

    qInfo() << "";
    qInfo() << "USAGE:";

    for (const QString &id : catalog.ids())
    {
        qInfo().noquote() << QStringLiteral("%1\t%2").arg(id).arg(usageIndex.usageCount(id));
    }

    QStringList missingIds;

    for (const QString &id : usageIndex.ids())
    {
        if (!catalog.contains(id))
        {
            missingIds.append(id);
        }
    }

    qInfo() << "";
    qInfo() << "UNUSED IDS:";

    for (const QString &id : catalog.ids())
    {
        if (usageIndex.usageCount(id) == 0)
        {
            qInfo().noquote() << id;
        }
    }

    qInfo() << "";
    qInfo() << "MISSING IDS:";

    for (const QString &id : missingIds)
    {
        qInfo().noquote() << id;
    }

    qInfo() << "";
    qInfo() << "DYNAMIC REFERENCES:";

    for (const LocalizationDynamicReference &reference :
         usageIndex.dynamicReferences())
    {
        qInfo().noquote() << QStringLiteral("%1:%2:%3").arg(reference.filePath).arg(reference.line).arg(reference.column);
    }

    return 0;
}