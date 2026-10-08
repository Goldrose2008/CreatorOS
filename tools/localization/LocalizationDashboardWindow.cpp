#include "LocalizationDashboardWindow.h"
#include "LocalizationTsvStore.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSplitter>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QCloseEvent>

LocalizationDashboardWindow::LocalizationDashboardWindow(LocalizationCatalog &catalog, const LocalizationUsageIndex &usageIndex, const QString &localizationPath, QWidget *parent)
    : QMainWindow(parent), catalog_(catalog), usageIndex_(usageIndex), localizationPath_(localizationPath)
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

    auto *filterLayout = new QHBoxLayout;

    filterLayout->addWidget(searchEdit_);

    statusFilter_ = new QComboBox(centralWidget);
    statusFilter_->addItem(QStringLiteral("Все"), QStringLiteral("all"));
    statusFilter_->addItem(QStringLiteral("Используется"), QStringLiteral("used"));
    statusFilter_->addItem(QStringLiteral("Не используется"), QStringLiteral("unused"));

    filterLayout->addWidget(statusFilter_);
    
    addButton_ = new QPushButton(QStringLiteral("Добавить"), centralWidget);
    filterLayout->addWidget(addButton_);

    deleteButton_ = new QPushButton(QStringLiteral("Удалить"), centralWidget);
    deleteButton_->setEnabled(false);

filterLayout->addWidget(deleteButton_);

    saveButton_ = new QPushButton(QStringLiteral("Сохранить"), centralWidget);
    saveButton_->setEnabled(false);
    filterLayout->addWidget(saveButton_);

    layout->addLayout(filterLayout);

    summaryLabel_ = new QLabel(centralWidget);

    layout->addWidget(summaryLabel_);

    auto *catalogPanel = new QWidget(centralWidget);
    auto *catalogLayout = new QVBoxLayout(catalogPanel);

    table_ = new QTableWidget(catalogPanel);

    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    table_->setAlternatingRowColors(true);
    table_->setSortingEnabled(true);

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
        statusFilter_,
        &QComboBox::currentIndexChanged,
        this,
        [this](int)
        {
            applyCatalogFilters();
        });

    connect(
        table_,
        &QTableWidget::itemSelectionChanged,
        this,
        [this]()
        {
            showSelectedUsage();
            updateDeleteButtonState();
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

    connect(
        table_,
        &QTableWidget::itemChanged,
        this,
        [this](QTableWidgetItem *item)
        {
            onTranslationChanged(item);
        });

    connect(
        addButton_,
        &QPushButton::clicked,
        this,
        [this]()
        {
            addEntry();
        });

    connect(
        deleteButton_,
        &QPushButton::clicked,
        this,
        [this]()
        {
            deleteEntry();
        });

    connect(
        saveButton_,
        &QPushButton::clicked,
        this,
        [this]()
        {
            saveCatalog();
        });
}

void LocalizationDashboardWindow::populateTable()
{
    const QStringList locales = catalog_.locales();

    const int usageColumn = 1 + locales.size();
    const int statusColumn = usageColumn + 1;

    QStringList headers;
    headers.append(QStringLiteral("Идентификатор"));

    for (const QString &locale : locales)
    {
        headers.append(locale.toUpper());
    }

    headers.append(QStringLiteral("Использований"));
    headers.append(QStringLiteral("Статус"));

    table_->setSortingEnabled(false);
    table_->setColumnCount(statusColumn + 1);
    table_->setHorizontalHeaderLabels(headers);
    table_->setRowCount(0);

    table_->horizontalHeader()->setStretchLastSection(false);
    table_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

    for (int column = 1; column <= locales.size(); ++column)
    {
        table_->horizontalHeader()->setSectionResizeMode(column, QHeaderView::Stretch);
    }

    table_->horizontalHeader()->setSectionResizeMode(usageColumn, QHeaderView::ResizeToContents);
    table_->horizontalHeader()->setSectionResizeMode(statusColumn, QHeaderView::ResizeToContents);

    for (const QString &id : catalog_.ids())
    {
        const LocalizationEntry *entry = catalog_.find(id);
        if (!entry){continue;}

        const int row = table_->rowCount();
        table_->insertRow(row);

        auto *idItem = new QTableWidgetItem(id);
        idItem->setFlags(idItem->flags() & ~Qt::ItemIsEditable);
        table_->setItem(row, 0, idItem);

        for (int localeIndex = 0; localeIndex < locales.size(); ++localeIndex)
        {
            const QString locale = locales.at(localeIndex);
            table_->setItem(row, localeIndex + 1, new QTableWidgetItem(entry->translations.value(locale)));
        }

        const int usageCount = usageIndex_.usageCount(id);

        auto *usageItem = new QTableWidgetItem(QString::number(usageCount));
        usageItem->setTextAlignment(Qt::AlignCenter);
        usageItem->setFlags(usageItem->flags() & ~Qt::ItemIsEditable);
        table_->setItem(row, usageColumn, usageItem);

        auto *statusItem = new QTableWidgetItem(usageCount > 0 ? QStringLiteral("Используется") : QStringLiteral("Не используется"));
        statusItem->setFlags(statusItem->flags() & ~Qt::ItemIsEditable);
        table_->setItem(row, statusColumn, statusItem);
    }

    table_->setSortingEnabled(true);
    applyCatalogFilters();

    summaryLabel_->setText(
        QStringLiteral(
            "Записей: %1   |   Статических обращений: %2   |   Динамических обращений: %3")
            .arg(catalog_.size()).arg(usageIndex_.staticUsageCount())
            .arg(usageIndex_.dynamicReferences().size()));
}

void LocalizationDashboardWindow::filterRows(const QString &text)
{
    Q_UNUSED(text);
    applyCatalogFilters();
}

void LocalizationDashboardWindow::applyCatalogFilters()
{
    const QString searchText = searchEdit_->text().trimmed();
    const QString status = statusFilter_->currentData().toString();
    const int statusColumn = table_->columnCount() - 1;

    for (int row = 0; row < table_->rowCount(); ++row)
    {
        const bool matchesSearch = searchText.isEmpty() || [&]()
            {
                for (int column = 0; column < table_->columnCount(); ++column)
                {
                    const QTableWidgetItem *item = table_->item(row, column);

                    if (item && item->text().contains(searchText, Qt::CaseInsensitive))
                    {
                        return true;
                    }
                }

                return false;
            }();

        const QString rowStatus = table_->item(row, statusColumn) ? table_->item(row, statusColumn)->text() : QString();
        bool matchesStatus = true;

        if (status == QStringLiteral("used"))
        {
            matchesStatus = rowStatus == QStringLiteral("Используется");
        }
        else if (status == QStringLiteral("unused"))
        {
            matchesStatus = rowStatus == QStringLiteral("Не используется");
        }

        table_->setRowHidden(row, !(matchesSearch && matchesStatus));
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

        auto *lineItem = new QTableWidgetItem(QString::number(usage.line));
        lineItem->setTextAlignment(Qt::AlignCenter);
        usageTable_->setItem(row, 1, lineItem);

        auto *columnItem = new QTableWidgetItem(QString::number(usage.column));
        columnItem->setTextAlignment(Qt::AlignCenter);
        usageTable_->setItem(row, 2, columnItem);
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

    const int problemCount = problemsTable_->rowCount();
    problemsTitleLabel_->setText(problemCount == 0 ? QStringLiteral("Problems: нет проблем") : QStringLiteral("Problems: %1").arg(problemCount));
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

void LocalizationDashboardWindow::onTranslationChanged(QTableWidgetItem *item)
{
    if (!item){return;}

    const int column = item->column();
    if (column <= 0){return;}

    const int localeCount = catalog_.locales().size();
    if (column > localeCount){return;}

    QTableWidgetItem *idItem = table_->item(item->row(), 0);
    if (!idItem){return;}

    const QString id = idItem->text();
    const QString locale = catalog_.locales().at(column - 1);
    QString error;

    if (!catalog_.updateTranslation(id, locale, item->text(), &error))
    {
        QMessageBox::critical(this, QStringLiteral("Ошибка"), error);
        populateTable();
        return;
    }

    updateDirtyState(true);
}

void LocalizationDashboardWindow::updateDirtyState(bool dirty)
{
    dirty_ = dirty;
    saveButton_->setEnabled(dirty_);
    setWindowTitle(dirty_ ? QStringLiteral("CreatorOS — Панель локализации *") : QStringLiteral("CreatorOS — Панель локализации"));
}

bool LocalizationDashboardWindow::saveCatalog()
{
    if (!dirty_){return true;}

    QString error;

    if (!LocalizationTsvStore::save(localizationPath_, catalog_, &error))
    {
        QMessageBox::critical(this, QStringLiteral("Ошибка сохранения"), error);
        return false;
    }

    updateDirtyState(false);
    QMessageBox::information(this, QStringLiteral("Сохранение"), QStringLiteral("Изменения успешно сохранены."));
    return true;
}

void LocalizationDashboardWindow::addEntry()
{
    QDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("Новая запись локализации"));
    auto *layout = new QFormLayout(&dialog);
    auto *idEdit = new QLineEdit(&dialog);
    layout->addRow(QStringLiteral("Идентификатор:"), idEdit);
    const QStringList locales = catalog_.locales();
    QMap<QString, QLineEdit *> translationEdits;

    for (const QString &locale : locales)
    {
        auto *edit = new QLineEdit(&dialog);
        translationEdits.insert(locale, edit);
        layout->addRow(QStringLiteral("%1:").arg(locale.toUpper()), edit);
    }

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(buttons);

    connect(
        buttons,
        &QDialogButtonBox::accepted,
        &dialog,
        [&]()
        {
            const QString id = idEdit->text().trimmed();

            if (id.isEmpty())
            {
                QMessageBox::warning(
                    &dialog,
                    QStringLiteral("Новая запись"),
                    QStringLiteral("Идентификатор не может быть пустым."));

                idEdit->setFocus();
                return;
            }

            if (catalog_.contains(id))
            {
                QMessageBox::warning(
                    &dialog,
                    QStringLiteral("Новая запись"),
                    QStringLiteral("Идентификатор уже существует: %1").arg(id));

                idEdit->setFocus();
                idEdit->selectAll();

                return;
            }

            dialog.accept();
        });

    connect(
        buttons,
        &QDialogButtonBox::rejected,
        &dialog,
        &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted){return;}

    LocalizationEntry entry;
    entry.id = idEdit->text().trimmed();

    for (const QString &locale : locales)
    {
        entry.translations.insert(locale, translationEdits.value(locale)->text());
    }

    QString error;

    if (!catalog_.addEntry(entry, &error))
    {
        QMessageBox::critical(this, QStringLiteral("Ошибка"), error);
        return;
    }

    updateDirtyState(true);

    populateTable();
    populateProblems();

    const QList<QTableWidgetItem *> matches = table_->findItems(entry.id, Qt::MatchExactly);

    if (!matches.isEmpty())
    {
        QTableWidgetItem *item = matches.first();
        table_->setCurrentItem(item);
        table_->scrollToItem(item);
        table_->selectRow(item->row());
    }

    updateDeleteButtonState();
}

void LocalizationDashboardWindow::updateDeleteButtonState()
{
    deleteButton_->setEnabled(!table_->selectedItems().isEmpty());
}

void LocalizationDashboardWindow::deleteEntry()
{
    const QList<QTableWidgetItem *> selectedItems = table_->selectedItems();

    if (selectedItems.isEmpty()){return;}

    const int row = selectedItems.first()->row();
    QTableWidgetItem *idItem = table_->item(row, 0);
    if (!idItem){return;}

    const QString id = idItem->text().trimmed();
    if (id.isEmpty()){return;}

    const int usageCount = usageIndex_.usageCount(id);
    QString message = QStringLiteral("Удалить запись «%1»?").arg(id);

    if (usageCount > 0)
    {
        message += QStringLiteral(
                "\n\nВ исходном коде найдено использований: %1."
                "\nУдаление создаст отсутствующий localization ID.").arg(usageCount);
    }

    const QMessageBox::StandardButton result = QMessageBox::warning(this, QStringLiteral("Удаление локализации"), message, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (result != QMessageBox::Yes){return;}

    QString error;

    if (!catalog_.removeEntry(id, &error))
    {
        QMessageBox::critical(this, QStringLiteral("Ошибка удаления"), error);
        return;
    }

    updateDirtyState(true);
    populateTable();
    populateProblems();
    usageTable_->setRowCount(0);
    usageTitleLabel_->setText(QStringLiteral("Места использования"));
    updateDeleteButtonState();
}

void LocalizationDashboardWindow::closeEvent(QCloseEvent *event)
{
    if (!dirty_)
    {
        event->accept();
        return;
    }

    QMessageBox messageBox(this);

    messageBox.setIcon(QMessageBox::Warning);
    messageBox.setWindowTitle(QStringLiteral("Несохранённые изменения"));
    messageBox.setText(QStringLiteral("В каталоге есть несохранённые изменения."));
    messageBox.setInformativeText(QStringLiteral("Что сделать перед закрытием панели локализации?"));

    QPushButton *saveButton = messageBox.addButton(QStringLiteral("Сохранить"), QMessageBox::AcceptRole);
    QPushButton *discardButton = messageBox.addButton(QStringLiteral("Не сохранять"), QMessageBox::DestructiveRole);

    messageBox.addButton(QStringLiteral("Отмена"), QMessageBox::RejectRole);
    messageBox.exec();

    if (messageBox.clickedButton() == saveButton)
    {
        if (saveCatalog())
        {
            event->accept();
        }
        else
        {
            event->ignore();
        }

        return;
    }

    if (messageBox.clickedButton() == discardButton)
    {
        event->accept();
        return;
    }

    event->ignore();
}

