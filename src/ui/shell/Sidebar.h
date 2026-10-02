#pragma once

#include <QString>
#include <QWidget>

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

signals:
    void routeTriggered(
        const QString &route,
        const QString &text
    );

private:
    QVBoxLayout *layout_;
    QButtonGroup *buttonGroup_;
};