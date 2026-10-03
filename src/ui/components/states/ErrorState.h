#pragma once

#include "StateWidget.h"

class QPushButton;

class ErrorState final : public StateWidget
{
    Q_OBJECT

public:
    explicit ErrorState(QWidget *parent = nullptr);

    void setRetryText(const QString &text);

signals:
    void retryRequested();

private:
    QPushButton *retryButton_;
};