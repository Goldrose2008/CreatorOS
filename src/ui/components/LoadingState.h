#pragma once

#include <QWidget>

class QLabel;

class LoadingState final : public QWidget
{
    Q_OBJECT

public:
    explicit LoadingState(QWidget *parent = nullptr);

    void setText(const QString &text);

private:
    QLabel *label_;
};