#pragma once

#include <QIcon>

enum class IconId
{
    Home,
    Projects,
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