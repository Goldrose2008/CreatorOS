#include "LocalizationDashboardWindow.h"
#include "LocalizationRefactorService.h"
#include "LocalizationTsvStore.h"
#include "LocalizationValidator.h"

#include <QBrush>
#include <QColor>
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
#include <QStringList>

LocalizationDashboardWindow::LocalizationDashboardWindow(LocalizationCatalog &catalog, LocalizationUsageIndex &usageIndex, const QString &localizationPath, const QString &sourceRoot, QWidget *parent)
    : QMainWindow(parent), catalog_(catalog), usageIndex_(usageIndex), localizationPath_(localizationPath), sourceRoot_(sourceRoot)
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

    renameButton_ = new QPushButton(QStringLiteral("Переименовать"), centralWidget);
    renameButton_->setEnabled(false);
    filterLayout->addWidget(renameButton_);

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

    problemsTitleLabel_ = new QLabel(QStringLiteral("Проблемы"), problemsPanel);
    problemsLayout->addWidget(problemsTitleLabel_);
    auto *problemsFilterLayout = new QHBoxLayout;
    problemsFilterLayout->addWidget(new QLabel(QStringLiteral("Показывать:"), problemsPanel));
    problemsSeverityFilter_ = new QComboBox(problemsPanel);
    problemsSeverityFilter_->addItem(QStringLiteral("Все"), QStringLiteral("all"));
    problemsSeverityFilter_->addItem(QStringLiteral("Только ошибки"), QStringLiteral("ERROR"));
    problemsSeverityFilter_->addItem(QStringLiteral("Только предупреждения"), QStringLiteral("WARNING"));
    problemsSeverityFilter_->addItem(QStringLiteral("Только информацию"), QStringLiteral("INFO"));
    problemsFilterLayout->addWidget(problemsSeverityFilter_);
    problemsFilterLayout->addStretch();
    problemsLayout->addLayout(problemsFilterLayout);
    problemsTable_ = new QTableWidget(problemsPanel);
    problemsTable_->setColumnCount(4);
    problemsTable_->setHorizontalHeaderLabels(
        {
            QStringLiteral("Важность"),
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
    problemsTable_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    problemsTable_->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
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
            updateRenameButtonState();
        });

    connect(
        problemsTable_,
        &QTableWidget::itemSelectionChanged,
        this,
        [this]()
        {
            selectProblemTarget();
        });

    connect(
        problemsSeverityFilter_,
        &QComboBox::currentIndexChanged,
        this,
        [this](int)
        {
            problemsTable_->clearSelection();
            applyProblemsSeverityFilter();
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
        renameButton_,
        &QPushButton::clicked,
        this,
        [this]()
        {
            renameEntry();
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

    LocalizationValidator validator;
    const QVector<LocalizationValidationIssue> issues = validator.validate(catalog_, usageIndex_);

    int errorCount = 0;
    int warningCount = 0;
    int infoCount = 0;

    for (const LocalizationValidationIssue &issue : issues)
    {
        QString severityText;
        QColor severityColor(255, 249, 196);

        switch (issue.severity)
        {
        case LocalizationValidationSeverity::Error: severityText = QStringLiteral("ERROR");
            severityColor = QColor(255, 205, 210);
            ++errorCount;
            break;
        case LocalizationValidationSeverity::Warning: severityText = QStringLiteral("WARNING");
            severityColor = QColor(255, 224, 178);
            ++warningCount;
            break;
        case LocalizationValidationSeverity::Info: severityText = QStringLiteral("INFO");
            severityColor = QColor(255, 249, 196);
            ++infoCount;
            break;
        }

        QString typeText;

        switch (issue.type)
        {
        case LocalizationValidationType::MissingId: typeText = QStringLiteral("MISSING");
            break;
        case LocalizationValidationType::InvalidIdFormat: typeText = QStringLiteral("INVALID_ID_FORMAT");
            break;
        case LocalizationValidationType::UnusedId: typeText = QStringLiteral("UNUSED");
            break;
        case LocalizationValidationType::EmptyTranslation: typeText = QStringLiteral("EMPTY_TRANSLATION");
            break;
        case LocalizationValidationType::DuplicateTranslation: typeText = QStringLiteral("DUPLICATE_TRANSLATION");
            break;
        case LocalizationValidationType::PlaceholderMismatch: typeText = QStringLiteral("PLACEHOLDER_MISMATCH");
            break;
        case LocalizationValidationType::DynamicReference: typeText = QStringLiteral("DYNAMIC");
            break;
        }

        QString locationText;

        if (!issue.filePath.isEmpty())
        {
            locationText = QStringLiteral("%1:%2:%3").arg(issue.filePath).arg(issue.line).arg(issue.column);
        }
        else if (!issue.id.isEmpty() && !issue.locale.isEmpty())
        {
            locationText = QStringLiteral("%1 [%2]").arg(issue.id, issue.locale);
        }
        else if (!issue.id.isEmpty())
        {
            locationText = issue.id;
        }
        else
        {
            locationText = QStringLiteral("—");
        }

        const int row = problemsTable_->rowCount();
        problemsTable_->insertRow(row);
        problemsTable_->setItem(row, 0, new QTableWidgetItem(severityText));
        problemsTable_->setItem(row, 1, new QTableWidgetItem(typeText));

        auto *locationItem = new QTableWidgetItem(locationText);

        if (!issue.id.isEmpty())
        {
            locationItem->setData(Qt::UserRole, issue.id);
        }

        problemsTable_->setItem(row, 2, locationItem);
        problemsTable_->setItem(row, 3, new QTableWidgetItem(issue.message));

        for (int column = 0; column < problemsTable_->columnCount(); ++column)
        {
            QTableWidgetItem *cell = problemsTable_->item(row, column);
            if (cell){cell->setBackground(QBrush(severityColor));}
        }
    }

    if (issues.isEmpty())
    {
        problemsTitleLabel_->setText(QStringLiteral("Проблем нет"));
    }
    else
    {
        problemsTitleLabel_->setText(QStringLiteral("Всего проблем: %1 | Ошибки: %2 | Предупреждения: %3 | Информация: %4").arg(issues.size()).arg(errorCount).arg(warningCount).arg(infoCount));
    }
    
    applyProblemsSeverityFilter();
}

void LocalizationDashboardWindow::applyProblemsSeverityFilter()
{
    if (!problemsSeverityFilter_ || !problemsTable_){return;}

    const QString selectedSeverity = problemsSeverityFilter_->currentData().toString();
    const bool showAll = selectedSeverity == QStringLiteral("all");

    for (int row = 0; row < problemsTable_->rowCount(); ++row)
    {
        const QTableWidgetItem *severityItem = problemsTable_->item(row, 0);
        const QString rowSeverity = severityItem ? severityItem->text() : QString();
        const bool matchesSeverity = showAll || rowSeverity == selectedSeverity;
        problemsTable_->setRowHidden(row, !matchesSeverity);
    }
}

void LocalizationDashboardWindow::selectProblemTarget()
{
    const QList<QTableWidgetItem *> selectedItems = problemsTable_->selectedItems();
    if (selectedItems.isEmpty()){return;}

    const QTableWidgetItem *locationItem = problemsTable_->item(selectedItems.first()->row(), 2);
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
    populateProblems();
}

void LocalizationDashboardWindow::updateDirtyState(bool dirty)
{
    dirty_ = dirty;
    saveButton_->setEnabled(dirty_);
    setWindowTitle(dirty_ ? QStringLiteral("CreatorOS — Панель локализации *") : QStringLiteral("CreatorOS — Панель локализации"));
}

bool LocalizationDashboardWindow::validateBeforeWrite(const QString &operationName)
{
    LocalizationValidator validator;
    const QVector<LocalizationValidationIssue> issues = validator.validate(catalog_, usageIndex_);
    QStringList errors;
    QStringList warnings;

    for (const LocalizationValidationIssue &issue : issues)
    {
        QString location = issue.id;

        if (!issue.locale.isEmpty())
        {
            location += QStringLiteral(" [%1]").arg(issue.locale);
        }

        if (!issue.filePath.isEmpty())
        {
            location = QStringLiteral("%1:%2:%3").arg(issue.filePath).arg(issue.line).arg(issue.column);
        }

        if (location.isEmpty())
        {
            location = QStringLiteral("—");
        }

        const QString description = QStringLiteral("%1 — %2").arg(location, issue.message);

        if (issue.severity == LocalizationValidationSeverity::Error)
        {
            errors.append(description);
        }
        else if (issue.severity == LocalizationValidationSeverity::Warning)
        {
            warnings.append(description);
        }
    }

    const auto formatIssues = [](const QStringList &items, int limit)
        {
            QStringList visibleItems;
            const int count = qMin(items.size(), limit);

            for (int index = 0; index < count; ++index)
            {
                visibleItems.append(QStringLiteral("• %1").arg(items.at(index)));
            }

            if (items.size() > limit)
            {
                visibleItems.append(QStringLiteral("… и ещё %1").arg(items.size() - limit));
            }

            return visibleItems.join('\n');
        };

    if (!errors.isEmpty())
    {
        QMessageBox::critical(
            this,
            QStringLiteral("Проверка локализации"),
            QStringLiteral("Операция «%1» отменена.\nОбнаружено ошибок: %2.\n\n%3\n\nИсправьте ошибки перед сохранением.").arg(operationName).arg(errors.size()).arg(formatIssues(errors, 10)));

        return false;
    }

    if (!warnings.isEmpty())
    {
        QMessageBox messageBox(this);
        messageBox.setIcon(QMessageBox::Warning);
        messageBox.setWindowTitle(QStringLiteral("Предупреждения локализации"));
        messageBox.setText(QStringLiteral("Перед операцией «%1» обнаружено предупреждений: %2.").arg(operationName).arg(warnings.size()));
        messageBox.setInformativeText(formatIssues(warnings, 10) + QStringLiteral("\n\nПродолжить операцию несмотря на предупреждения?"));
        QPushButton *continueButton = messageBox.addButton(QStringLiteral("Продолжить"), QMessageBox::AcceptRole);
        QPushButton *cancelButton = messageBox.addButton(QStringLiteral("Отмена"), QMessageBox::RejectRole);
        messageBox.setDefaultButton(cancelButton);
        messageBox.exec();
        return messageBox.clickedButton() == continueButton;
    }

    return true;
}

bool LocalizationDashboardWindow::saveCatalog()
{
    if (!dirty_){return true;}
    if (!validateBeforeWrite(QStringLiteral("сохранение"))){return false;}
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

void LocalizationDashboardWindow::updateRenameButtonState()
{
    renameButton_->setEnabled(!table_->selectedItems().isEmpty());
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

void LocalizationDashboardWindow::renameEntry()
{
    const QList<QTableWidgetItem *> selectedItems = table_->selectedItems();
    if (selectedItems.isEmpty()){return;}

    if (dirty_)
    {
        QMessageBox::warning(
            this,
            QStringLiteral("Переименование"),
            QStringLiteral("Перед переименованием сначала сохраните текущие изменения каталога."));

        return;
    }

    const int row = selectedItems.first()->row();
    QTableWidgetItem *idItem = table_->item(row, 0);
    if (!idItem){return;}

    const QString oldId = idItem->text().trimmed();
    if (oldId.isEmpty()){return;}

    QDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("Переименование локализации"));

    auto *layout = new QFormLayout(&dialog);
    auto *newIdEdit = new QLineEdit(&dialog);

    layout->addRow(QStringLiteral("Текущий ID:"), new QLabel(oldId, &dialog));
    layout->addRow(QStringLiteral("Новый ID:"), newIdEdit);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(buttons);

    connect(
        buttons,
        &QDialogButtonBox::accepted,
        &dialog,
        [&]()
        {
            const QString newId = newIdEdit->text().trimmed();

            if (newId.isEmpty())
            {
                QMessageBox::warning(&dialog, QStringLiteral("Переименование"), QStringLiteral("Новый идентификатор не может быть пустым."));
                newIdEdit->setFocus();
                return;
            }

            if (newId == oldId)
            {
                QMessageBox::warning(&dialog, QStringLiteral("Переименование"), QStringLiteral("Новый идентификатор должен отличаться от текущего."));
                newIdEdit->setFocus();
                newIdEdit->selectAll();
                return;
            }

            if (catalog_.contains(newId))
            {
                QMessageBox::warning(&dialog, QStringLiteral("Переименование"), QStringLiteral("Идентификатор уже существует: %1").arg(newId));
                newIdEdit->setFocus();
                newIdEdit->selectAll();
                return;
            }

            dialog.accept();
        });

    connect(
        buttons,
        &QDialogButtonBox::rejected,
        &dialog,
        &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }

    const QString newId = newIdEdit->text().trimmed();

    LocalizationRefactorService refactorService;
    LocalizationRenamePreview preview;
    QString error;

    if (!refactorService.previewRename(sourceRoot_, usageIndex_, oldId, newId, preview, &error))
    {
        QMessageBox::critical(this, QStringLiteral("Ошибка preview"), error);
        return;
    }

    QDialog previewDialog(this);
    previewDialog.setWindowTitle(QStringLiteral("Preview переименования"));
    previewDialog.resize(1000, 600);

    auto *previewLayout = new QVBoxLayout(&previewDialog);
    previewLayout->addWidget(new QLabel(QStringLiteral("ID каталога: %1 → %2").arg(oldId, newId), &previewDialog));

    auto *changesLabel = new QLabel(QStringLiteral("Статических изменений в исходниках: %1").arg(preview.changes.size()), &previewDialog);
    previewLayout->addWidget(changesLabel);

    auto *changesTable = new QTableWidget(&previewDialog);
    changesTable->setColumnCount(5);
    changesTable->setHorizontalHeaderLabels(
        {
            QStringLiteral("Файл"),
            QStringLiteral("Строка"),
            QStringLiteral("Столбец"),
            QStringLiteral("До"),
            QStringLiteral("После")
        });

    changesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    changesTable->setSelectionMode(QAbstractItemView::SingleSelection);
    changesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    changesTable->setAlternatingRowColors(true);
    changesTable->setWordWrap(false);

    changesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    changesTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    changesTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    changesTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    changesTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);

    for (const LocalizationRenameChange &change : preview.changes)
    {
        const int previewRow = changesTable->rowCount();
        changesTable->insertRow(previewRow);

        changesTable->setItem(previewRow, 0, new QTableWidgetItem(change.filePath));
        auto *lineItem = new QTableWidgetItem(QString::number(change.line));

        lineItem->setTextAlignment(Qt::AlignCenter);
        changesTable->setItem(previewRow, 1, lineItem);

        auto *columnItem = new QTableWidgetItem(QString::number(change.column));
        columnItem->setTextAlignment(Qt::AlignCenter);
        changesTable->setItem(previewRow, 2, columnItem);

        changesTable->setItem(previewRow, 3, new QTableWidgetItem(change.beforeLine));
        changesTable->setItem(previewRow, 4, new QTableWidgetItem(change.afterLine));
    }

    previewLayout->addWidget(changesTable);

    if (!preview.dynamicReferences.isEmpty())
    {
        previewLayout->addWidget(
            new QLabel(
                QStringLiteral(
                    "Предупреждение: обнаружено динамических localization-ссылок: %1. "
                    "Они не будут изменены автоматически.").arg(preview.dynamicReferences.size()), &previewDialog));
    }

    if (preview.changes.isEmpty())
    {
        previewLayout->addWidget(new QLabel(QStringLiteral(
                    "В исходном коде статических обращений к этому ID не найдено. "
                    "Будет изменён только ID в каталоге."), &previewDialog));
    }

    auto *previewButtons = new QDialogButtonBox(QDialogButtonBox::Cancel, &previewDialog);
    QPushButton *applyButton = previewButtons->addButton(QStringLiteral("Применить"), QDialogButtonBox::AcceptRole);
    applyButton->setDefault(true);
    previewLayout->addWidget(previewButtons);

    connect(
        previewButtons,
        &QDialogButtonBox::accepted,
        &previewDialog,
        &QDialog::accept);

    connect(
        previewButtons, 
        &QDialogButtonBox::rejected, 
        &previewDialog, 
        &QDialog::reject);

    if (previewDialog.exec() != QDialog::Accepted){return;}
    if (!validateBeforeWrite(QStringLiteral("переименование"))){return;}
    if (!refactorService.applyRename(sourceRoot_, localizationPath_, catalog_, usageIndex_, oldId, newId, &error))
    {
        QMessageBox::critical(this, QStringLiteral("Ошибка переименования"), error);
        return;
    }

    updateDirtyState(false);
    populateTable();
    populateProblems();
    searchEdit_->clear();

    const QList<QTableWidgetItem *> matches = table_->findItems(newId, Qt::MatchExactly);

    if (!matches.isEmpty())
    {
        QTableWidgetItem *item = matches.first();
        table_->setCurrentItem(item);
        table_->scrollToItem(item);
        table_->selectRow(item->row());
    }

    updateDeleteButtonState();
    updateRenameButtonState();
    QMessageBox::information(
        this,
        QStringLiteral("Переименование"),
        QStringLiteral("Localization ID успешно переименован:\n%1 → %2").arg(oldId, newId));
}

