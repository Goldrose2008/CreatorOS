#pragma once

#include <QObject>
#include <QHash>
#include <QString>

class LocalizationService final : public QObject
{
    Q_OBJECT

public:
    explicit LocalizationService(QObject *parent = nullptr);
    bool load(const QString &resourcePath);
    QString text(const QString &id) const;
    QString locale() const;
    bool setLocale(const QString &locale);

private:
    QHash<QString, int> columnIndexes_;
    QHash<QString, QHash<QString, QString>> entries_;
    QString currentLocale_{"ru"};
};