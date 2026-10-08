#include "LocalizationDashboardWindow.h"

#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QWidget>
#include <QSplitter>
#include <QAbstractItemView>

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

    auto *catalogPanel = new QWidget(centralWidget);
    auto *catalogLayout = new QVBoxLayout(catalogPanel);

    table_ = new QTableWidget(catalogPanel);
    table_->setColumnCount(5);
    table_->setHorizontalHeaderLabels(
        {
            QStringLiteral("Идентификатор"),
            QStringLiteral("Русский"),
            QStringLiteral("Английский"),
            QStringLiteral("Использований"),
            QStringLiteral("Статус")
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
    table_->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);

    catalogLayout->addWidget(table_);

    auto *detailsSplitter = new QSplitter(Qt::Vertical, centralWidget);

    auto *usagePanel = new QWidget(detailsSplitter);
    auto *usageLayout = new QVBoxLayout(usagePanel);

    usageTitleLabel_ = new QLabel(QStringLiteral("Места использования"), usagePanel);
    usageLayout->addWidget(usageTitleLabel_);
    usageTable_ = new QTableWidget(usagePanel);
    usageTable_->setColumnCount(3);
    usageTable_->setHorizontalHeaderLabels(
        {
            QStringLiteral("Файл"),
            QStringLiteral("Строка"),
            QStringLiteral("Столбец")
        });

    usageTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    usageTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    usageTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    usageTable_->setAlternatingRowColors(true);
    usageTable_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    usageTable_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    usageTable_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    usageLayout->addWidget(usageTable_);

    auto *problemsPanel = new QWidget(detailsSplitter);
    auto *problemsLayout = new QVBoxLayout(problemsPanel);

    problemsTitleLabel_ = new QLabel(QStringLiteral("Problems"), problemsPanel);
    problemsLayout->addWidget(problemsTitleLabel_);
    problemsTable_ = new QTableWidget(problemsPanel);
    problemsTable_->setColumnCount(3);
    problemsTable_->setHorizontalHeaderLabels(
        {
            QStringLiteral("Тип"),
            QStringLiteral("Место"),
            QStringLiteral("Описание")
        });
    problemsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    problemsTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    problemsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    problemsTable_->setAlternatingRowColors(true);
    problemsTable_->setWordWrap(true);

    problemsTable_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    problemsTable_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    problemsTable_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    problemsLayout->addWidget(problemsTable_);

    detailsSplitter->addWidget(usagePanel);
    detailsSplitter->addWidget(problemsPanel);
    detailsSplitter->setStretchFactor(0, 1);
    detailsSplitter->setStretchFactor(1, 1);

    auto *mainSplitter = new QSplitter(Qt::Vertical, centralWidget);

    mainSplitter->addWidget(catalogPanel);
    mainSplitter->addWidget(detailsSplitter);
    mainSplitter->setStretchFactor(0, 3);
    mainSplitter->setStretchFactor(1, 2);

    layout->addWidget(mainSplitter);

    setCentralWidget(centralWidget);

    connect(
        searchEdit_,
        &QLineEdit::textChanged,
        this,
        [this](const QString &text)
        {
            filterRows(text);
        });

    connect(
        table_,
        &QTableWidget::itemSelectionChanged,
        this,
        [this]()
        {
            showSelectedUsage();
        });
    
    connect(
        problemsTable_,
        &QTableWidget::itemSelectionChanged,
        this,
        [this]()
        {
            selectProblemTarget();
        });

    populateTable();
    populateProblems();
}

void LocalizationDashboardWindow::populateTable()
{
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
        const int usageCount = usageIndex_.usageCount(id);

        table_->setItem(row, 0, new QTableWidgetItem(id));
        table_->setItem(row, 1, new QTableWidgetItem(ru));
        table_->setItem(row, 2, new QTableWidgetItem(en));
        table_->setItem(row, 3, new QTableWidgetItem(QString::number(usageCount)));
        table_->setItem(row, 4, new QTableWidgetItem(usageCount > 0 ? QStringLiteral("Используется") : QStringLiteral("Не используется")));
    }

    table_->setSortingEnabled(true);

    summaryLabel_->setText(
        QStringLiteral("Записей: %1   |   Статических обращений: %2   |   Динамических обращений: %3")
            .arg(catalog_.size()).arg(usageIndex_.staticUsageCount()).arg(usageIndex_.dynamicReferences().size()));
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

void LocalizationDashboardWindow::showSelectedUsage()
{
    usageTable_->setRowCount(0);

    const QList<QTableWidgetItem *> selectedItems = table_->selectedItems();

    if (selectedItems.isEmpty()){return;}

    const QString id = selectedItems.first()->text();
    const QVector<LocalizationUsage> usages = usageIndex_.usagesFor(id);

    for (const LocalizationUsage &usage : usages)
    {
        const int row = usageTable_->rowCount();
        usageTable_->insertRow(row);

        usageTable_->setItem(row, 0, new QTableWidgetItem(usage.filePath));
        usageTable_->setItem(row, 1, new QTableWidgetItem(QString::number(usage.line)));
        usageTable_->setItem(row, 2, new QTableWidgetItem(QString::number(usage.column)));
    }

    usageTitleLabel_->setText(QStringLiteral("Места использования: %1").arg(id));
}

void LocalizationDashboardWindow::populateProblems()
{
    problemsTable_->setRowCount(0);

    for (const QString &id : catalog_.ids())
    {
        if (usageIndex_.usageCount(id) != 0){continue;}

        const int row = problemsTable_->rowCount();
        problemsTable_->insertRow(row);

        problemsTable_->setItem(row, 0, new QTableWidgetItem(QStringLiteral("UNUSED")));
        auto *locationItem = new QTableWidgetItem(id);
        locationItem->setData(Qt::UserRole, id);
        problemsTable_->setItem(row, 1, locationItem);
        problemsTable_->setItem(row, 2, new QTableWidgetItem(QStringLiteral("ID есть в каталоге, но статических использований не найдено.")));
    }

    for (const QString &id : usageIndex_.ids())
    {
        if (catalog_.contains(id)){continue;}

        const QVector<LocalizationUsage> usages = usageIndex_.usagesFor(id);

        for (const LocalizationUsage &usage : usages)
        {
            const int row = problemsTable_->rowCount();
            problemsTable_->insertRow(row);

            problemsTable_->setItem(row, 0, new QTableWidgetItem(QStringLiteral("MISSING")));
            auto *locationItem = new QTableWidgetItem(QStringLiteral("%1:%2:%3").arg(usage.filePath).arg(usage.line).arg(usage.column));
            locationItem->setData(Qt::UserRole, id);
            problemsTable_->setItem(row, 1, locationItem);
            problemsTable_->setItem(row, 2, new QTableWidgetItem(QStringLiteral("Источник использует ID, отсутствующий в каталоге: %1").arg(id)));
        }
    }

    for (const LocalizationDynamicReference &reference :
         usageIndex_.dynamicReferences())
    {
        const int row = problemsTable_->rowCount();
        problemsTable_->insertRow(row);

        problemsTable_->setItem(row, 0, new QTableWidgetItem(QStringLiteral("DYNAMIC")));
        problemsTable_->setItem(row, 1, new QTableWidgetItem(QStringLiteral("%1:%2:%3").arg(reference.filePath).arg(reference.line).arg(reference.column)));
        problemsTable_->setItem(row, 2, new QTableWidgetItem(QStringLiteral("Динамическая localization-ссылка не может быть автоматически сопоставлена с ID.")));
    }

    problemsTitleLabel_->setText(QStringLiteral("Problems: %1").arg(problemsTable_->rowCount()));
}

void LocalizationDashboardWindow::selectProblemTarget()
{
    const QList<QTableWidgetItem *> selectedItems = problemsTable_->selectedItems();
    if (selectedItems.isEmpty()){return;}

    const QTableWidgetItem *locationItem = problemsTable_->item(selectedItems.first()->row(), 1);
    if (!locationItem){return;}

    const QVariant idValue = locationItem->data(Qt::UserRole);
    if (!idValue.isValid()){return;}

    const QString id = idValue.toString();
    if (id.isEmpty()){return;}

    searchEdit_->clear();
    const QList<QTableWidgetItem *> matches = table_->findItems(id, Qt::MatchExactly);
    if (matches.isEmpty()){return;}

    QTableWidgetItem *item = matches.first();

    table_->setCurrentItem(item);
    table_->scrollToItem(item);
    table_->selectRow(item->row());
}

