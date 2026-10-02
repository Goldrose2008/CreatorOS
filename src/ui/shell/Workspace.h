#pragma once

#include <QStackedWidget>

class QLabel;

class Workspace final : public QStackedWidget
{
    Q_OBJECT

public:
    explicit Workspace(QWidget *parent = nullptr);

    void setPageTitle(const QString &title);

private:
    QLabel *placeholderTitle_;
};