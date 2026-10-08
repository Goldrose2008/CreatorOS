#include "LocalizationDashboardWindow.h"

#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QWidget>

LocalizationDashboardWindow::LocalizationDashboardWindow(const LocalizationCatalog &catalog, const LocalizationUsageIndex &usageIndex, QWidget *parent)
    : QMainWindow(parent), catalog_(catalog), usageIndex_(usageIndex)
{
    setWindowTitle(QStringLiteral("CreatorOS — Панель локализации"));
    resize(1100, 700);

    auto *centralWidget = new QWidget(this);
    auto *layout = new QVBoxLayout(centralWidget);

    auto *titleLabel = new QLabel(QStringLiteral("Панель локализации"), centralWidget);

    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(titleFont.pointSize() + 4);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    layout->addWidget(titleLabel);

    searchEdit_ = new QLineEdit(centralWidget);
    searchEdit_->setPlaceholderText(QStringLiteral("Поиск по ID или переводу..."));

    layout->addWidget(searchEdit_);

    summaryLabel_ = new QLabel(centralWidget);
    layout->addWidget(summaryLabel_);

    table_ = new QTableWidget(centralWidget);
    table_->setColumnCount(4);
    table_->setHorizontalHeaderLabels(
        {
            QStringLiteral("Идентификатор"),
            QStringLiteral("Русский"),
            QStringLiteral("Английский"),
            QStringLiteral("Использований")
        });

    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setAlternatingRowColors(true);
    table_->setSortingEnabled(true);

    table_->horizontalHeader()->setStretchLastSection(false);
    table_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    table_->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);

    layout->addWidget(table_);

    setCentralWidget(centralWidget);

    connect(
        searchEdit_,
        &QLineEdit::textChanged,
        this,
        [this](const QString &text)
        {
            filterRows(text);
        });

    populateTable();
}

void LocalizationDashboardWindow::populateTable()
{
    const QStringList locales = catalog_.locales();

    table_->setSortingEnabled(false);
    table_->setRowCount(0);

    for (const QString &id : catalog_.ids())
    {
        const LocalizationEntry *entry = catalog_.find(id);

        if (!entry){continue;}

        const int row = table_->rowCount();
        table_->insertRow(row);

        const QString ru = entry->translations.value(QStringLiteral("ru"));
        const QString en = entry->translations.value(QStringLiteral("en"));

        table_->setItem(row, 0, new QTableWidgetItem(id));
        table_->setItem(row, 1, new QTableWidgetItem(ru));
        table_->setItem(row, 2, new QTableWidgetItem(en));
        table_->setItem(row, 3, new QTableWidgetItem(QString::number(usageIndex_.usageCount(id))));
    }

    table_->setSortingEnabled(true);

    summaryLabel_->setText(
        QStringLiteral("Записей: %1   |   Статических обращений: %2   |   Динамических обращений: %3")
            .arg(catalog_.size()).arg(usageIndex_.staticUsageCount()).arg(usageIndex_.dynamicReferences().size()));

    Q_UNUSED(locales);
}

void LocalizationDashboardWindow::filterRows(const QString &text)
{
    const QString searchText = text.trimmed();

    for (int row = 0; row < table_->rowCount(); ++row)
    {
        bool visible = searchText.isEmpty();

        if (!visible)
        {
            for (int column = 0; column < table_->columnCount(); ++column)
            {
                const QTableWidgetItem *item = table_->item(row, column);

                if (item && item->text().contains(searchText, Qt::CaseInsensitive))
                {
                    visible = true;
                    break;
                }
            }
        }

        table_->setRowHidden(row, !visible);
    }
}