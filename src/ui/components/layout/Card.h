#pragma once

#include "../foundation/SurfaceWidget.h"

class Card : public SurfaceWidget
{
    Q_OBJECT

public:
    explicit Card(QWidget *parent = nullptr);

protected:
    QColor fillColor() const override;
};