#include "Section.h"

#include <QFont>
#include <QLabel>
#include <QVBoxLayout>

#include "../../style/Colors.h"
#include "../../style/Metrics.h"

Section::Section(QWidget *parent)
    : QWidget(parent),
      titleLabel_(new QLabel(this)),
      layout_(new QVBoxLayout(this))
{
    layout_->setContentsMargins(0, 0, 0, 0);

    layout_->setSpacing(CreatorMetrics::SpacingMedium);

    QFont titleFont = titleLabel_->font();

    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel_->setFont(titleFont);

    QPalette palette = titleLabel_->palette();

    palette.setColor(QPalette::WindowText, CreatorColors::TextPrimary);
    titleLabel_->setPalette(palette);
    layout_->addWidget(titleLabel_);
}

void Section::setTitle(const QString &title)
{
    titleLabel_->setText(title);
}

QVBoxLayout *Section::contentLayout() const
{
    return layout_;
}