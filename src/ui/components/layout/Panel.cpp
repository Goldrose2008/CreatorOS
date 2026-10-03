#include "Panel.h"

#include "../../style/Colors.h"

Panel::Panel(QWidget *parent) : SurfaceWidget(parent)
{
}

QColor Panel::fillColor() const
{
    return CreatorColors::Surface;
}