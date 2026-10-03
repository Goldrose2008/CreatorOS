#include "MenuSection.h"

#include <QFont>
#include <QLabel>
#include <QPalette>
#include <QVBoxLayout>

#include "../../style/Colors.h"
#include "../../style/Metrics.h"

MenuSection::MenuSection(QWidget *parent) : QWidget(parent), label_(new QLabel(this))
{
    auto *layout = new QVBoxLayout(this);

    layout->setContentsMargins(
        CreatorMetrics::SpacingSmall,
        CreatorMetrics::SpacingXLarge,
        CreatorMetrics::SpacingSmall,
        CreatorMetrics::SpacingSmall
    );

    layout->addWidget(label_);

    QFont sectionFont = label_->font();

    sectionFont.setPointSize(9);
    sectionFont.setBold(true);

    label_->setFont(sectionFont);

    QPalette palette = label_->palette();

    palette.setColor(QPalette::WindowText, CreatorColors::TextMuted);

    label_->setPalette(palette);
}

void MenuSection::setText(const QString &text)
{
    label_->setText(text);
}