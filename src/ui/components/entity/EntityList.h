#pragma once

#include <QScrollArea>

class QVBoxLayout;
class QWidget;

class EntityList final : public QScrollArea
{
    Q_OBJECT

public:
    explicit EntityList(QWidget *parent = nullptr);

    void addItem(QWidget *item);
    void clearItems();
    int count() const;

private:
    QWidget *contentWidget_;
    QVBoxLayout *layout_;
    int itemCount_;
};