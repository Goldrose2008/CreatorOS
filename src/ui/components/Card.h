#pragma once

#include <QWidget>

class QVBoxLayout;

class Card final : public QWidget
{
    Q_OBJECT

public:
    explicit Card(QWidget *parent = nullptr);

    QVBoxLayout *contentLayout() const;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVBoxLayout *layout_;
};