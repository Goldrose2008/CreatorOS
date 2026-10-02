#pragma once

#include <QHash>
#include <QObject>
#include <QString>

#include "CurrentPage.h"

class Workspace;

class NavigationController final : public QObject
{
    Q_OBJECT

public:
    explicit NavigationController(Workspace *workspace, QObject *parent = nullptr);

    bool addPage(const CurrentPage &page);
    bool navigateTo(const QString &route);

    QString currentRoute() const;

signals:
    void currentPageChanged(const QString &route);

private:
    Workspace *workspace_;
    QHash<QString, CurrentPage> pages_;
    QString currentRoute_;
};