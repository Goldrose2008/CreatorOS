#include "Card.h"

#include "../../style/Colors.h"

Card::Card(QWidget *parent) : SurfaceWidget(parent)
{
}

QColor Card::fillColor() const
{
    return CreatorColors::SurfaceElevated;
}