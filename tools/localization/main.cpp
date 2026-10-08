#include "LocalizationDashboardWindow.h"
#include "LocalizationSourceScanner.h"
#include "LocalizationTsvStore.h"

#include <QApplication>
#include <QDebug>
#include <QStringList>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

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

    LocalizationDashboardWindow window(catalog, usageIndex, localizationPath);
    window.show();

    return application.exec();
}