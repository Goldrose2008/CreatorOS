#pragma once

#include <QWidget>

class AccountService;
class PlatformService;
class LocalizationService;
class EntityList;

class AccountsPage final : public QWidget
{
    Q_OBJECT

public:
    explicit AccountsPage(
        AccountService &accountService,
        PlatformService &platformService,
        LocalizationService &localization,
        QWidget *parent = nullptr
    );

    void reload();

private:
    AccountService &accountService_;
    PlatformService &platformService_;
    LocalizationService &localization_;

    EntityList *accountList_;
};