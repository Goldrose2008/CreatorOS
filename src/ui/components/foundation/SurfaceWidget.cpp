#include "SurfaceWidget.h"

#include <QPainter>
#include <QVBoxLayout>

#include "../../style/Colors.h"
#include "../../style/Metrics.h"

SurfaceWidget::SurfaceWidget(QWidget *parent)
    : QWidget(parent),
      layout_(new QVBoxLayout(this))
{
    setAttribute(Qt::WA_StyledBackground, true);

    layout_->setContentsMargins(
        CreatorMetrics::SpacingLarge,
        CreatorMetrics::SpacingLarge,
        CreatorMetrics::SpacingLarge,
        CreatorMetrics::SpacingLarge
    );

    layout_->setSpacing(CreatorMetrics::SpacingMedium);
}

QVBoxLayout *SurfaceWidget::contentLayout() const
{
    return layout_;
}

void SurfaceWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    const QRectF rect{
        0.5,
        0.5,
        width() - 1.0,
        height() - 1.0
    };

    painter.setBrush(fillColor());
    painter.setPen(CreatorColors::Border);
    painter.drawRoundedRect(
        rect,
        CreatorMetrics::CornerRadius,
        CreatorMetrics::CornerRadius
    );
}