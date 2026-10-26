#pragma once

#include <QIcon>

enum class IconId
{
    Home,
    Projects,
    Accounts,
    Platforms,
    Planning,
    Tasks,
    Library,
    Analytics,
    Settings
};

namespace Icons
{
    QIcon get(IconId id);
}