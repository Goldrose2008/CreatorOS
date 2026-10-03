#include "EntityCard.h"

#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QProgressBar>

#include "../../style/Colors.h"
#include "../../style/Metrics.h"

EntityCard::EntityCard(QWidget *parent)
    : Card(parent),
      titleLabel_(new QLabel(this)),
      descriptionLabel_(new QLabel(this)),
      titleLayout_(new QHBoxLayout()),
      metaLayout_(new QHBoxLayout()),
      actionsLayout_(new QHBoxLayout()),
      progressBar_(new QProgressBar(this)),
      statusWidget_(nullptr)
{
    titleLayout_->setContentsMargins(0, 0, 0, 0);
    titleLayout_->setSpacing(CreatorMetrics::SpacingMedium);

    metaLayout_->setContentsMargins(0, 0, 0, 0);
    metaLayout_->setSpacing(CreatorMetrics::SpacingSmall);

    actionsLayout_->setContentsMargins(0, 0, 0, 0);
    actionsLayout_->setSpacing(CreatorMetrics::SpacingSmall);

    QFont titleFont = titleLabel_->font();
    titleFont.setPointSize(16);
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

    progressBar_->setRange(0, 100);
    progressBar_->setVisible(false);
    progressBar_->setTextVisible(false);

    contentLayout()->addLayout(titleLayout_);
    contentLayout()->addWidget(descriptionLabel_);
    contentLayout()->addWidget(progressBar_);
    contentLayout()->addLayout(metaLayout_);
    contentLayout()->addLayout(actionsLayout_);
}

void EntityCard::setTitle(const QString &title)
{
    titleLabel_->setText(title);
}

void EntityCard::setDescription(const QString &description)
{
    descriptionLabel_->setText(description);
    descriptionLabel_->setVisible(!description.isEmpty());
}

void EntityCard::setStatusWidget(QWidget *widget)
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

void EntityCard::addContentWidget(QWidget *widget)
{
    if (widget == nullptr){return;}

    widget->setParent(this);
    contentLayout()->insertWidget(contentLayout()->count() - 3, widget);
}

void EntityCard::addMetaWidget(QWidget *widget)
{
    if (widget == nullptr){return;}

    widget->setParent(this);
    const int insertIndex = metaLayout_->count() - 1;
    metaLayout_->insertWidget(insertIndex, widget);
}

void EntityCard::setProgress(int value)
{
    if (value < 0)
    {
        value = 0;
    }
    else if (value > 100)
    {
        value = 100;
    }

    progressBar_->setValue(value);
    progressBar_->setVisible(true);
}

void EntityCard::clearProgress()
{
    progressBar_->setVisible(false);
}

void EntityCard::addAction(QWidget *widget)
{
    if (widget == nullptr){return;}

    widget->setParent(this);
    actionsLayout_->addWidget(widget);
}