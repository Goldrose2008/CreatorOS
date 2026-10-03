#include "EntityHeader.h"

#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QVBoxLayout>

#include "../../style/Colors.h"
#include "../../style/Metrics.h"

EntityHeader::EntityHeader(QWidget *parent)
    : QWidget(parent),
      titleLabel_(new QLabel(this)),
      descriptionLabel_(new QLabel(this)),
      titleLayout_(new QHBoxLayout()),
      metaLayout_(new QHBoxLayout()),
      actionsLayout_(new QHBoxLayout()),
      textLayout_(new QVBoxLayout()),
      layout_(new QHBoxLayout(this)),
      statusWidget_(nullptr)
{
    layout_->setContentsMargins(0, 0, 0, 0);
    layout_->setSpacing(CreatorMetrics::SpacingLarge);

    textLayout_->setContentsMargins(0, 0, 0, 0);
    textLayout_->setSpacing(CreatorMetrics::SpacingSmall);

    titleLayout_->setContentsMargins(0, 0, 0, 0);
    titleLayout_->setSpacing(CreatorMetrics::SpacingMedium);

    metaLayout_->setContentsMargins(0, 0, 0, 0);
    metaLayout_->setSpacing(CreatorMetrics::SpacingSmall);

    actionsLayout_->setContentsMargins(0, 0, 0, 0);
    actionsLayout_->setSpacing(CreatorMetrics::SpacingSmall);

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
    descriptionLabel_->setVisible(false);

    titleLayout_->addWidget(titleLabel_);
    titleLayout_->addStretch();

    metaLayout_->addStretch();

    textLayout_->addLayout(titleLayout_);
    textLayout_->addWidget(descriptionLabel_);
    textLayout_->addLayout(metaLayout_);

    layout_->addLayout(textLayout_, 1);
    layout_->addLayout(actionsLayout_);
}

void EntityHeader::setTitle(const QString &title)
{
    titleLabel_->setText(title);
}

void EntityHeader::setDescription(const QString &description)
{
    descriptionLabel_->setText(description);
    descriptionLabel_->setVisible(!description.isEmpty());
}

void EntityHeader::setStatusWidget(QWidget *widget)
{
    if (statusWidget_ == widget){return;}

    if (statusWidget_ != nullptr)
    {
        titleLayout_->removeWidget(statusWidget_);
        statusWidget_->deleteLater();
        statusWidget_ = nullptr;
    }

    if (widget == nullptr){return;}

    widget->setParent(this);

    const int insertIndex = titleLayout_->count() - 1;

    titleLayout_->insertWidget(insertIndex, widget);
    statusWidget_ = widget;
}

void EntityHeader::addMetaWidget(QWidget *widget)
{
    if (widget == nullptr){return;}

    widget->setParent(this);
    const int insertIndex = metaLayout_->count() - 1;
    metaLayout_->insertWidget(insertIndex, widget);
}

void EntityHeader::addAction(QWidget *widget)
{
    if (widget == nullptr){return;}

    widget->setParent(this);
    actionsLayout_->addWidget(widget);
}