#pragma once

#include <QSqlDatabase>
#include <QString>

class DatabaseManager
{
public:
    DatabaseManager();
    ~DatabaseManager();

    bool initialize(QString *errorMessage = nullptr);

    QSqlDatabase database() const;
    QString databasePath() const;

private:
    bool applySchema(QString *errorMessage);

    QSqlDatabase database_;
    QString connectionName_;
    QString databasePath_;
};