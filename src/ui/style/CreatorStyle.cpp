#include "CreatorStyle.h"

#include <QPalette>
#include <QStyleFactory>

#include "Colors.h"

CreatorStyle::CreatorStyle() : QProxyStyle(QStyleFactory::create("Fusion"))
{
}

void CreatorStyle::polish(QPalette &palette)
{
    QProxyStyle::polish(palette);

    palette.setColor(QPalette::Window, CreatorColors::WindowBackground);
    palette.setColor(QPalette::Base, CreatorColors::Surface);
    palette.setColor(QPalette::AlternateBase, CreatorColors::SurfaceElevated);

    palette.setColor(QPalette::Text, CreatorColors::TextPrimary);
    palette.setColor(QPalette::WindowText, CreatorColors::TextPrimary);
    palette.setColor(QPalette::PlaceholderText, CreatorColors::TextMuted);

    palette.setColor(QPalette::Button, CreatorColors::SurfaceElevated);
    palette.setColor(QPalette::ButtonText, CreatorColors::TextPrimary);

    palette.setColor(QPalette::Highlight, CreatorColors::Accent);
    palette.setColor(QPalette::HighlightedText, CreatorColors::WindowBackground);

    palette.setColor(QPalette::Mid, CreatorColors::Border);
    palette.setColor(QPalette::Dark, CreatorColors::WindowBackground);
    palette.setColor(QPalette::Light, CreatorColors::SurfaceElevated);
}