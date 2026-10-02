#pragma once

#include <QWidget>

class LocalizationService;
class Sidebar;
class Workspace;

class AppShell final : public QWidget
{
    Q_OBJECT

public:
    explicit AppShell(QWidget *parent = nullptr);

private:
    LocalizationService *localization_;
    Sidebar *sidebar_;
    Workspace *workspace_;
};