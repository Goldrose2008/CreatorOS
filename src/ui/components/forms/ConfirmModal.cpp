#include "ConfirmModal.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "../../style/Metrics.h"

ConfirmModal::ConfirmModal(
    const QString &title,
    const QString &description,
    const QString &confirmText,
    const QString &cancelText,
    QWidget *parent
)
    : QDialog(parent)
{
    setWindowTitle(title);
    resize(480, 220);

    auto *layout = new QVBoxLayout(this);

    layout->setContentsMargins(
        CreatorMetrics::SpacingXLarge,
        CreatorMetrics::SpacingXLarge,
        CreatorMetrics::SpacingXLarge,
        CreatorMetrics::SpacingXLarge
    );

    layout->setSpacing(CreatorMetrics::SpacingLarge);

    auto *descriptionLabel = new QLabel(description, this);
    descriptionLabel->setWordWrap(true);

    layout->addWidget(descriptionLabel);
    layout->addStretch();

    auto *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Cancel | QDialogButtonBox::Ok,
        this
    );

    buttonBox->button(QDialogButtonBox::Ok)->setText(confirmText);
    buttonBox->button(QDialogButtonBox::Cancel)->setText(cancelText);

    connect(
        buttonBox,
        &QDialogButtonBox::accepted,
        this,
        &ConfirmModal::accept
    );

    connect(
        buttonBox,
        &QDialogButtonBox::rejected,
        this,
        &ConfirmModal::reject
    );

    layout->addWidget(buttonBox);
}

void ConfirmModal::confirm(
    QWidget *parent,
    const QString &title,
    const QString &description,
    const QString &confirmText,
    const QString &cancelText,
    std::function<void()> onConfirmed
)
{
    ConfirmModal dialog(
        title,
        description,
        confirmText,
        cancelText,
        parent
    );

    if (dialog.exec() == QDialog::Accepted && onConfirmed)
    {
        onConfirmed();
    }
}