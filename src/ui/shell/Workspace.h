#pragma once

#include <QStackedWidget>

class Workspace final : public QStackedWidget
{
    Q_OBJECT

public:
    explicit Workspace(QWidget *parent = nullptr);
};