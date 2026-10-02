#pragma once

#include <QString>
#include <QWidget>
#include <QList>

class QButtonGroup;
class QVBoxLayout;
class MenuItem;
class MenuSection;

class Sidebar final : public QWidget
{
    Q_OBJECT

public:
    explicit Sidebar(QWidget *parent = nullptr);

    void addSection(MenuSection *section);
    void addItem(MenuItem *item);
    void setActiveRoute(const QString &route);

signals:
    void routeTriggered(const QString &route);

private:
    QVBoxLayout *layout_;
    QButtonGroup *buttonGroup_;
    QList<MenuItem *> items_;
};