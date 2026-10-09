#include "../LocalizationCatalog.h"
#include "../LocalizationUsageIndex.h"
#include "../LocalizationValidator.h"

#include <QDebug>
#include <QString>

namespace
{
    int failedTests = 0;
    int passedTests = 0;

    void check(const QString &name, bool passed)
    {
        if (passed)
        {
            ++passedTests;
            qInfo().noquote() << "PASS:" << name;
        }
        else
        {
            ++failedTests;
            qCritical().noquote() << "FAIL:" << name;
        }
    }

    void addEntry(LocalizationCatalog &catalog, const QString &id, const QString &ru, const QString &en)
    {
        LocalizationEntry entry;
        entry.id = id;
        entry.translations.insert(QStringLiteral("ru"), ru);
        entry.translations.insert(QStringLiteral("en"), en);

        QString error;

        if (!catalog.addEntry(entry, &error))
        {
            qCritical().noquote() << "Test setup failed:" << error; ++failedTests;
        }
    }

    bool hasIssue(
        const QVector<LocalizationValidationIssue> &issues,
        LocalizationValidationType type,
        const QString &id,
        const QString &locale,
        LocalizationValidationSeverity severity,
        const QString &filePath = QString(),
        int line = 0,
        int column = 0)
    {
        for (const LocalizationValidationIssue &issue : issues)
        {
            if (issue.type != type || issue.severity != severity){continue;}
            if (!id.isEmpty() && issue.id != id){continue;}
            if (!locale.isEmpty() && issue.locale != locale){continue;}
            if (!filePath.isEmpty() && issue.filePath != filePath){continue;}
            if (line > 0 && issue.line != line){continue;}
            if (column > 0 && issue.column != column){continue;}

            return true;
        }

        return false;
    }
}

int main()
{
    LocalizationValidator validator;

    // 1. Корректный каталог не должен давать проблем.
    {
        LocalizationCatalog catalog;
        catalog.setLocales({QStringLiteral("ru"), QStringLiteral("en")});
        addEntry(catalog, QStringLiteral("greeting"), QStringLiteral("Привет, %1"), QStringLiteral("Hello, %1"));
        LocalizationUsageIndex usageIndex;
        usageIndex.addStaticUsage(QStringLiteral("greeting"), LocalizationUsage{QStringLiteral("src/Greeting.cpp"), 1, 20});
        const auto issues = validator.validate(catalog, usageIndex);
        check(QStringLiteral("Valid translations produce no issues"), issues.isEmpty());
    }

    // 2. Пустой перевод должен давать ERROR.
    {
        LocalizationCatalog catalog;
        catalog.setLocales({QStringLiteral("ru"), QStringLiteral("en")});
        addEntry(catalog, QStringLiteral("empty.translation"), QStringLiteral("Привет"), QStringLiteral("   "));
        LocalizationUsageIndex usageIndex;
        usageIndex.addStaticUsage(QStringLiteral("empty.translation"), LocalizationUsage{QStringLiteral("src/Test.cpp"), 1, 1});
        const auto issues = validator.validate(catalog, usageIndex);
        check(QStringLiteral("Empty translation is an error"), hasIssue(issues, LocalizationValidationType::EmptyTranslation, QStringLiteral("empty.translation"), QStringLiteral("en"), LocalizationValidationSeverity::Error));
    }

    // 3. Дубликаты должны находиться без учёта регистра и лишних пробелов.
    {
        LocalizationCatalog catalog;
        catalog.setLocales({QStringLiteral("ru"), QStringLiteral("en")});
        addEntry(catalog, QStringLiteral("action.save"), QStringLiteral("Сохранить"), QStringLiteral("Save"));
        addEntry(catalog, QStringLiteral("menu.save"), QStringLiteral("  СОХРАНИТЬ  "), QStringLiteral("SAVE"));
        LocalizationUsageIndex usageIndex;
        usageIndex.addStaticUsage(QStringLiteral("action.save"), LocalizationUsage{QStringLiteral("src/Actions.cpp"), 1, 1});
        usageIndex.addStaticUsage(QStringLiteral("menu.save"), LocalizationUsage{QStringLiteral("src/Menu.cpp"), 1, 1});
        const auto issues = validator.validate(catalog, usageIndex);
        const bool russianDuplicate = hasIssue(issues, LocalizationValidationType::DuplicateTranslation, QStringLiteral("menu.save"), QStringLiteral("ru"), LocalizationValidationSeverity::Warning);
        const bool englishDuplicate = hasIssue(issues, LocalizationValidationType::DuplicateTranslation, QStringLiteral("menu.save"), QStringLiteral("en"), LocalizationValidationSeverity::Warning);
        check(QStringLiteral("Duplicate translations are detected"), russianDuplicate && englishDuplicate);
    }

    // 4. Подстановки в переводах должны совпадать.
    {
        LocalizationCatalog catalog;
        catalog.setLocales({QStringLiteral("ru"), QStringLiteral("en")});
        addEntry(catalog, QStringLiteral("greeting"), QStringLiteral("Привет, %1"), QStringLiteral("Hello"));
        LocalizationUsageIndex usageIndex;
        usageIndex.addStaticUsage(QStringLiteral("greeting"), LocalizationUsage{QStringLiteral("src/Greeting.cpp"), 1, 1});
        const auto issues = validator.validate(catalog, usageIndex);
        check(QStringLiteral("Placeholder mismatch is detected"), hasIssue(issues, LocalizationValidationType::PlaceholderMismatch, QStringLiteral("greeting"), QStringLiteral("en"), LocalizationValidationSeverity::Error));
    }

    // 5. Отсутствующий ID должен включать координаты source.
    {
        LocalizationCatalog catalog;
        catalog.setLocales({QStringLiteral("ru"), QStringLiteral("en")});
        LocalizationUsageIndex usageIndex;
        usageIndex.addStaticUsage(QStringLiteral("missing.key"), LocalizationUsage{QStringLiteral("src/Example.cpp"), 12, 8});
        const auto issues = validator.validate(catalog, usageIndex);
        check(QStringLiteral("Missing ID is reported with source location"), hasIssue(issues, LocalizationValidationType::MissingId, QStringLiteral("missing.key"), QString(), LocalizationValidationSeverity::Error, QStringLiteral("src/Example.cpp"), 12, 8));
    }

    // 6. Неиспользуемый ID должен быть предупреждением.
    {
        LocalizationCatalog catalog;
        catalog.setLocales({QStringLiteral("ru"), QStringLiteral("en")});
        addEntry(catalog, QStringLiteral("unused.key"), QStringLiteral("Дополнение"), QStringLiteral("Additional"));
        LocalizationUsageIndex usageIndex;
        const auto issues = validator.validate(catalog, usageIndex);
        check(QStringLiteral("Unused ID is reported as a warning"), hasIssue(issues, LocalizationValidationType::UnusedId, QStringLiteral("unused.key"), QString(), LocalizationValidationSeverity::Warning));
    }

    // 7. Динамическая ссылка — предупреждение, а не Missing ID.
    {
        LocalizationCatalog catalog;
        catalog.setLocales({QStringLiteral("ru"), QStringLiteral("en")});
        LocalizationUsageIndex usageIndex;
        usageIndex.addDynamicReference(LocalizationDynamicReference{QStringLiteral("src/Dynamic.cpp"), 7, 4});
        const auto issues = validator.validate(catalog, usageIndex);
        const bool dynamicWarning = hasIssue(issues, LocalizationValidationType::DynamicReference, QString(), QString(), LocalizationValidationSeverity::Warning, QStringLiteral("src/Dynamic.cpp"), 7, 4);
        const bool noMissingId = !hasIssue(issues, LocalizationValidationType::MissingId, QString(), QString(), LocalizationValidationSeverity::Error);
        check(QStringLiteral("Dynamic reference stays a warning"), dynamicWarning && noMissingId);
    }

    qInfo() << "Tests passed:" << passedTests << "| failed:" << failedTests;
    return failedTests == 0 ? 0 : 1;
}