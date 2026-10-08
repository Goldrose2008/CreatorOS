#pragma once

#include "LocalizationCatalog.h"
#include "LocalizationUsageIndex.h"

#include <QMainWindow>

class QLabel;
class QComboBox;
class QCloseEvent;
class QLineEdit;
class QTableWidget;
class QTableWidgetItem;
class QPushButton;

class LocalizationDashboardWindow final : public QMainWindow
{
public:
    LocalizationDashboardWindow(LocalizationCatalog &catalog, const LocalizationUsageIndex &usageIndex, const QString &localizationPath, const QString &sourceRoot, QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;
    
private:
    void populateTable();
    void filterRows(const QString &text);
    void applyCatalogFilters();
    void showSelectedUsage();
    void populateProblems();
    void selectProblemTarget();
    void onTranslationChanged(QTableWidgetItem *item);
    bool saveCatalog();
    void addEntry();
    void deleteEntry();
    void updateDeleteButtonState();
    void updateRenameButtonState();
    void renameEntry();
    void updateDirtyState(bool dirty);

    LocalizationCatalog &catalog_;
    const LocalizationUsageIndex &usageIndex_;

    QLineEdit *searchEdit_ = nullptr;
    QPushButton *saveButton_ = nullptr;
    QPushButton *addButton_ = nullptr;
    QPushButton *deleteButton_ = nullptr;
    QPushButton *renameButton_ = nullptr;
    QComboBox *statusFilter_ = nullptr;
    QTableWidget *table_ = nullptr;
    QLabel *summaryLabel_ = nullptr;
    QString localizationPath_;
    QString sourceRoot_;
    bool dirty_ = false;
    QTableWidget *usageTable_ = nullptr;
    QTableWidget *problemsTable_ = nullptr;
    QLabel *usageTitleLabel_ = nullptr;
    QLabel *problemsTitleLabel_ = nullptr;
};