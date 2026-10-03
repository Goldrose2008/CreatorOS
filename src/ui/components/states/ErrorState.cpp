#include "ErrorState.h"

#include <QVBoxLayout>
#include <QPushButton>

ErrorState::ErrorState(QWidget *parent) : StateWidget(parent), retryButton_(new QPushButton(this))
{
    retryButton_->setVisible(false);

    contentLayout()->addWidget(retryButton_, 0, Qt::AlignCenter);
    connect(retryButton_, &QPushButton::clicked, this, &ErrorState::retryRequested);
}

void ErrorState::setRetryText(const QString &text)
{
    retryButton_->setText(text);
    retryButton_->setVisible(!text.isEmpty());
}