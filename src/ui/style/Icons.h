#pragma once

#include <QIcon>

enum class IconId
{
    Home,
    Projects,
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