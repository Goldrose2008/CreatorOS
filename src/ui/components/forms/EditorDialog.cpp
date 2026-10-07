#include "EditorDialog.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "../../style/Metrics.h"

EditorDialog::EditorDialog(
    Mode mode,
    const QString &title,
    const QString &saveText,
    const QString &cancelText,
    QWidget *parent
)
    : QDialog(parent),
      mode_(mode),
      layout_(new QVBoxLayout(this)),
      contentLayout_(new QVBoxLayout()),
      buttonBox_(
          new QDialogButtonBox(
              QDialogButtonBox::Cancel | QDialogButtonBox::Save,
              this
          )
      )
{
    setWindowTitle(title);

    layout_->setContentsMargins(
        CreatorMetrics::SpacingXLarge,
        CreatorMetrics::SpacingXLarge,
        CreatorMetrics::SpacingXLarge,
        CreatorMetrics::SpacingXLarge
    );

    layout_->setSpacing(CreatorMetrics::SpacingLarge);

    contentLayout_->setContentsMargins(0, 0, 0, 0);
    contentLayout_->setSpacing(CreatorMetrics::SpacingMedium);

    layout_->addLayout(contentLayout_);
    layout_->addStretch();
    layout_->addWidget(buttonBox_);

    buttonBox_->button(QDialogButtonBox::Save)->setText(saveText);
    buttonBox_->button(QDialogButtonBox::Cancel)->setText(cancelText);

    connect(
        buttonBox_,
        &QDialogButtonBox::accepted,
        this,
        &EditorDialog::handleSave
    );

    connect(
        buttonBox_,
        &QDialogButtonBox::rejected,
        this,
        &EditorDialog::reject
    );
}

QVBoxLayout *EditorDialog::contentLayout() const
{
    return contentLayout_;
}

bool EditorDialog::isEditMode() const
{
    return mode_ == Mode::Edit;
}

void EditorDialog::setSaveEnabled(bool enabled)
{
    buttonBox_->button(QDialogButtonBox::Save)->setEnabled(enabled);
}

void EditorDialog::handleSave()
{
    if (save())
    {
        accept();
    }
}