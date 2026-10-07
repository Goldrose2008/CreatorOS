#pragma once

#include <QDialog>

class QDialogButtonBox;
class QVBoxLayout;

class EditorDialog : public QDialog
{
    Q_OBJECT

public:
    enum class Mode
    {
        Create,
        Edit
    };

    explicit EditorDialog(
        Mode mode,
        const QString &title,
        const QString &saveText,
        const QString &cancelText,
        QWidget *parent = nullptr
    );

protected:
    QVBoxLayout *contentLayout() const;
    bool isEditMode() const;

    void setSaveEnabled(bool enabled);

    virtual bool save() = 0;

private slots:
    void handleSave();

private:
    Mode mode_;
    
    QVBoxLayout *layout_;
    QVBoxLayout *contentLayout_;
    QDialogButtonBox *buttonBox_;
};