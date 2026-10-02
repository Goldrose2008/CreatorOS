#include "Sidebar.h"

#include <QButtonGroup>
#include <QPalette>
#include <QVBoxLayout>

#include "../components/MenuItem.h"
#include "../components/MenuSection.h"
#include "../style/Colors.h"
#include "../style/Metrics.h"

Sidebar::Sidebar(QWidget *parent)
    : QWidget(parent),
      layout_(new QVBoxLayout(this)),
      buttonGroup_(new QButtonGroup(this))
{
    setObjectName(QStringLiteral("Sidebar"));
    setFixedWidth(CreatorMetrics::SidebarWidth);
    setAutoFillBackground(true);

    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, CreatorColors::Surface);

    setPalette(palette);

    layout_->setContentsMargins(
        CreatorMetrics::SpacingMedium,
        CreatorMetrics::SpacingMedium,
        CreatorMetrics::SpacingMedium,
        CreatorMetrics::SpacingMedium
    );

    layout_->setSpacing(CreatorMetrics::SpacingSmall);
    buttonGroup_->setExclusive(true);
}

void Sidebar::addSection(MenuSection *section)
{
    layout_->addWidget(section);
}

void Sidebar::addItem(MenuItem *item)
{
    buttonGroup_->addButton(item);
    layout_->addWidget(item);

    connect(
        item,
        &MenuItem::triggered,
        this,
        [this, item](const QString &route)
        {
            emit routeTriggered(
                route,
                item->text()
            );
        }
    );
}