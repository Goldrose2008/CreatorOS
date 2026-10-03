#pragma once

#include "../foundation/SurfaceWidget.h"

class Panel final : public SurfaceWidget
{
    Q_OBJECT

public:
    explicit Panel(QWidget *parent = nullptr);

protected:
    QColor fillColor() const override;
};