#include "PageHeader.h"

#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include "../../style/Colors.h"
#include "../../style/Metrics.h"

PageHeader::PageHeader(QWidget *parent)
    : QWidget(parent),
      titleLabel_(new QLabel(this)),
      descriptionLabel_(new QLabel(this)),
      layout_(new QHBoxLayout(this))
{
    auto *textLayout = new QVBoxLayout();

    textLayout->setContentsMargins(0, 0, 0, 0);
    textLayout->setSpacing(CreatorMetrics::SpacingSmall);

    QFont titleFont = titleLabel_->font();

    titleFont.setPointSize(24);
    titleFont.setBold(true);
    titleLabel_->setFont(titleFont);

    QPalette titlePalette = titleLabel_->palette();

    titlePalette.setColor(QPalette::WindowText, CreatorColors::TextPrimary);
    titleLabel_->setPalette(titlePalette);

    QPalette descriptionPalette = descriptionLabel_->palette();

    descriptionPalette.setColor(QPalette::WindowText, CreatorColors::TextSecondary);
    descriptionLabel_->setPalette(descriptionPalette);
    descriptionLabel_->setWordWrap(true);
    
    textLayout->addWidget(titleLabel_);
    textLayout->addWidget(descriptionLabel_);

    layout_->setContentsMargins(0, 0, 0, 0);

    layout_->setSpacing(CreatorMetrics::SpacingMedium);

    layout_->addLayout(textLayout, 1);
}

void PageHeader::setTitle(const QString &title)
{
    titleLabel_->setText(title);
}

void PageHeader::setDescription(const QString &description)
{
    descriptionLabel_->setText(description);
    descriptionLabel_->setVisible(!description.isEmpty());
}

void PageHeader::addAction(QWidget *widget)
{
    if (widget == nullptr)
    {
        return;
    }

    layout_->addWidget(widget);
}