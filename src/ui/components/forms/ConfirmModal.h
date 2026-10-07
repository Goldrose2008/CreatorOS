#pragma once

#include <QDialog>
#include <functional>

class QDialogButtonBox;
class QLabel;
class QVBoxLayout;

class ConfirmModal final : public QDialog
{
    Q_OBJECT

public:
    static void confirm(
        QWidget *parent,
        const QString &title,
        const QString &description,
        const QString &confirmText,
        const QString &cancelText,
        std::function<void()> onConfirmed
    );

private:
    explicit ConfirmModal(
        const QString &title,
        const QString &description,
        const QString &confirmText,
        const QString &cancelText,
        QWidget *parent = nullptr
    );
};