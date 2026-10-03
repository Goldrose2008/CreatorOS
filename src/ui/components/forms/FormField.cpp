#include "FormField.h"

#include <QFont>
#include <QLabel>
#include <QPalette>
#include <QVBoxLayout>

#include "../../style/Colors.h"
#include "../../style/Metrics.h"

FormField::FormField(QWidget *parent)
    : QWidget(parent),
      label_(new QLabel(this)),
      descriptionLabel_(new QLabel(this)),
      errorLabel_(new QLabel(this)),
      layout_(new QVBoxLayout(this))
{
    layout_->setContentsMargins(0, 0, 0, 0);

    layout_->setSpacing(CreatorMetrics::SpacingSmall);

    QFont labelFont = label_->font();

    labelFont.setBold(true);
    label_->setFont(labelFont);

    QPalette labelPalette = label_->palette();

    labelPalette.setColor(QPalette::WindowText, CreatorColors::TextPrimary);
    label_->setPalette(labelPalette);

    QPalette descriptionPalette = descriptionLabel_->palette();

    descriptionPalette.setColor(QPalette::WindowText, CreatorColors::TextSecondary);
    descriptionLabel_->setPalette(descriptionPalette);

    QPalette errorPalette = errorLabel_->palette();

    errorPalette.setColor(QPalette::WindowText, CreatorColors::Accent);
    errorLabel_->setPalette(errorPalette);
    descriptionLabel_->setWordWrap(true);
    errorLabel_->setWordWrap(true);
    descriptionLabel_->setVisible(false);
    errorLabel_->setVisible(false);
    layout_->addWidget(label_);
    layout_->addWidget(descriptionLabel_);
}

void FormField::setLabel(const QString &label)
{
    label_->setText(label);
}

void FormField::setDescription(const QString &description)
{
    descriptionLabel_->setText(description);
    descriptionLabel_->setVisible(!description.isEmpty());
}

void FormField::setError(const QString &error)
{
    errorLabel_->setText(error);
    errorLabel_->setVisible(!error.isEmpty());
}

void FormField::setField(QWidget *field)
{
    if (field == nullptr)
    {
        return;
    }

    layout_->insertWidget(1, field);
}