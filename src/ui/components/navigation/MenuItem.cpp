#include "MenuItem.h"

#include <QPainter>

#include "../../style/Colors.h"
#include "../../style/Metrics.h"

MenuItem::MenuItem(QWidget *parent) : QAbstractButton(parent)
{
    setCheckable(true);
    setAttribute(Qt::WA_Hover, true);
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::StrongFocus);

    connect(this, &QAbstractButton::clicked, this, [this] {emit triggered(route_);});
}

void MenuItem::setIcon(const QIcon &icon)
{
    icon_ = icon;
    update();
}

void MenuItem::setRoute(const QString &route)
{
    route_ = route;
}

void MenuItem::setActive(bool active)
{
    setChecked(active);
    update();
}

QString MenuItem::route() const
{
    return route_;
}

void MenuItem::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QRectF backgroundRect{
        0.5,
        0.5,
        width() - 1.0,
        height() - 1.0
    };

    if (isChecked())
    {
        painter.setBrush(CreatorColors::Active);
    }
    else if (underMouse())
    {
        painter.setBrush(CreatorColors::Hover);
    }
    else
    {
        painter.setBrush(Qt::NoBrush);
    }

    painter.setPen(Qt::NoPen);

    painter.drawRoundedRect(
        backgroundRect,
        CreatorMetrics::SmallCornerRadius,
        CreatorMetrics::SmallCornerRadius
    );

    constexpr int iconSize = 20;
    constexpr int iconLeft = 14;

    const int iconTop = (height() - iconSize) / 2;

    icon_.paint(
        &painter,
        QRect(iconLeft, iconTop, iconSize, iconSize),
        Qt::AlignCenter
    );

    QColor textColor = CreatorColors::TextSecondary;

    if (isChecked() || underMouse())
    {
        textColor = CreatorColors::TextPrimary;
    }

    if (!isEnabled())
    {
        textColor = CreatorColors::TextMuted;
    }

    painter.setPen(textColor);
    painter.setFont(font());

    const QRect textRect{
        48,
        0,
        width() - 60,
        height()
    };

    painter.drawText(
        textRect,
        Qt::AlignVCenter | Qt::AlignLeft,
        text()
    );
}

QSize MenuItem::sizeHint() const
{
    return {220, CreatorMetrics::MenuItemHeight};
}