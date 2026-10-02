#pragma once

#include <QWidget>

class QLabel;

class MenuSection final : public QWidget
{
    Q_OBJECT

public:
    explicit MenuSection(QWidget *parent = nullptr);

    void setText(const QString &text);

private:
    QLabel *label_;
};