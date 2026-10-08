#include "LocalizationTsvStore.h"

#include <QCoreApplication>
#include <QDebug>
#include <QStringList>

int main(int argc, char *argv[])
{
    QCoreApplication application(argc, argv);

    const QStringList arguments = application.arguments();

    if (arguments.size() != 2)
    {
        qCritical() << "Usage: CreatorOSLocalization <path-to-localization.tsv>";
        return 1;
    }

    LocalizationCatalog catalog;
    QString error;

    if (!LocalizationTsvStore::load(arguments.at(1), catalog, &error))
    {
        qCritical() << error;
        return 1;
    }

    qInfo() << "Localization catalog loaded successfully.";
    qInfo() << "Entries:" << catalog.size();
    qInfo() << "Locales:" << catalog.locales();
    qInfo() << "First IDs:" << catalog.ids().mid(0, 10);

    return 0;
}