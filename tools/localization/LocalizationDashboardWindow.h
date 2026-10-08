#pragma once

#include "LocalizationCatalog.h"
#include "LocalizationUsageIndex.h"

#include <QMainWindow>

class QLabel;
class QLineEdit;
class QTableWidget;

class LocalizationDashboardWindow final : public QMainWindow
{
public:
    LocalizationDashboardWindow(const LocalizationCatalog &catalog, const LocalizationUsageIndex &usageIndex, QWidget *parent = nullptr);

private:
    void populateTable();
    void filterRows(const QString &text);
    void showSelectedUsage();
    void populateProblems();

    const LocalizationCatalog &catalog_;
    const LocalizationUsageIndex &usageIndex_;

    QLineEdit *searchEdit_ = nullptr;
    QTableWidget *table_ = nullptr;
    QLabel *summaryLabel_ = nullptr;
    QTableWidget *usageTable_ = nullptr;
    QTableWidget *problemsTable_ = nullptr;
    QLabel *usageTitleLabel_ = nullptr;
    QLabel *problemsTitleLabel_ = nullptr;
};