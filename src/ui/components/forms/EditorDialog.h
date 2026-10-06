#pragma once

#include <QDialog>

class QDialogButtonBox;
class QVBoxLayout;

class EditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditorDialog(
        const QString &title,
        const QString &saveText,
        const QString &cancelText,
        QWidget *parent = nullptr
    );

protected:
    QVBoxLayout *contentLayout() const;

    void setSaveEnabled(bool enabled);

    virtual bool save() = 0;

private slots:
    void handleSave();

private:
    QVBoxLayout *layout_;
    QVBoxLayout *contentLayout_;
    QDialogButtonBox *buttonBox_;
};