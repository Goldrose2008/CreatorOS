#include "AccountRepository.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <stdexcept>

#include "DatabaseManager.h"

namespace
{
    Account readAccount(const QSqlQuery &query)
    {
        Account account;
        account.id = query.value("id").toLongLong();
        account.platformId = query.value("platform_id").toLongLong();
        account.name = query.value("name").toString().toStdString();
        account.username = query.value("username").toString().toStdString();
        account.profileUrl = query.value("profile_url").toString().toStdString();
        account.externalId = query.value("external_id").toString().toStdString();
        account.createdAt = query.value("created_at").toString().toStdString();
        account.updatedAt = query.value("updated_at").toString().toStdString();

        return account;
    }

    QString selectColumns()
    {
        return QStringLiteral(
            "id, "
            "platform_id, "
            "name, "
            "username, "
            "profile_url, "
            "external_id, "
            "created_at, "
            "updated_at "
        );
    }
}

AccountRepository::AccountRepository(const DatabaseManager &databaseManager) : database_(databaseManager.database())
{
}

std::vector<Account> AccountRepository::findAll() const
{
    QSqlQuery query(database_);

    query.prepare(
        QStringLiteral("SELECT ") + selectColumns() +
        QStringLiteral(
            "FROM accounts "
            "ORDER BY platform_id ASC, name COLLATE NOCASE ASC, id ASC"
        )
    );

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    std::vector<Account> accounts;

    while (query.next())
        accounts.push_back(readAccount(query));

    return accounts;
}

std::optional<Account> AccountRepository::findById(std::int64_t id) const
{
    QSqlQuery query(database_);

    query.prepare(
        QStringLiteral("SELECT ") + selectColumns() +
        QStringLiteral("FROM accounts WHERE id = :id")
    );

    query.bindValue(QStringLiteral(":id"), QVariant::fromValue(id));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    if (!query.next())
        return std::nullopt;

    return readAccount(query);
}

std::int64_t AccountRepository::create(const Account &account)
{
    QSqlQuery query(database_);

    query.prepare(
        QStringLiteral(
            "INSERT INTO accounts "
            "(platform_id, name, username, profile_url, external_id) "
            "VALUES "
            "(:platform_id, :name, :username, :profile_url, :external_id)"
        )
    );

    query.bindValue(QStringLiteral(":platform_id"), QVariant::fromValue(account.platformId));
    query.bindValue(QStringLiteral(":name"), QString::fromUtf8(account.name.c_str()));
    query.bindValue(QStringLiteral(":username"), QString::fromUtf8(account.username.c_str()));
    query.bindValue(QStringLiteral(":profile_url"), QString::fromUtf8(account.profileUrl.c_str()));
    query.bindValue(QStringLiteral(":external_id"), QString::fromUtf8(account.externalId.c_str()));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    return query.lastInsertId().toLongLong();
}

bool AccountRepository::update(const Account &account)
{
    QSqlQuery query(database_);

    query.prepare(
        QStringLiteral(
            "UPDATE accounts "
            "SET platform_id = :platform_id, "
            "name = :name, "
            "username = :username, "
            "profile_url = :profile_url, "
            "external_id = :external_id, "
            "updated_at = CURRENT_TIMESTAMP "
            "WHERE id = :id"
        )
    );

    query.bindValue(QStringLiteral(":platform_id"), QVariant::fromValue(account.platformId));
    query.bindValue(QStringLiteral(":name"), QString::fromUtf8(account.name.c_str()));
    query.bindValue(QStringLiteral(":username"), QString::fromUtf8(account.username.c_str()));
    query.bindValue(QStringLiteral(":profile_url"), QString::fromUtf8(account.profileUrl.c_str()));
    query.bindValue(QStringLiteral(":external_id"), QString::fromUtf8(account.externalId.c_str()));
    query.bindValue(QStringLiteral(":id"), QVariant::fromValue(account.id));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    return query.numRowsAffected() > 0;
}

bool AccountRepository::remove(std::int64_t id)
{
    QSqlQuery query(database_);

    query.prepare(QStringLiteral("DELETE FROM accounts WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), QVariant::fromValue(id));

    if (!query.exec())
        throw std::runtime_error(query.lastError().text().toStdString());

    return query.numRowsAffected() > 0;
}