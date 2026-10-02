#pragma once

#include <QWidget>

class QVBoxLayout;

class Panel final : public QWidget
{
    Q_OBJECT

public:
    explicit Panel(QWidget *parent = nullptr);

    QVBoxLayout *contentLayout() const;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVBoxLayout *layout_;
};