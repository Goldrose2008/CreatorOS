#pragma once

#include <QWidget>

class QHBoxLayout;

class Toolbar final : public QWidget
{
    Q_OBJECT

public:
    explicit Toolbar(QWidget *parent = nullptr);

    void addWidget(QWidget *widget);
    void addStretch();

private:
    QHBoxLayout *layout_;
};