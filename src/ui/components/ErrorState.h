#pragma once

#include <QWidget>

class QLabel;
class QPushButton;

class ErrorState final : public QWidget
{
    Q_OBJECT

public:
    explicit ErrorState(QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setDescription(const QString &description);
    void setRetryText(const QString &text);

signals:
    void retryRequested();

private:
    QLabel *titleLabel_;
    QLabel *descriptionLabel_;
    QPushButton *retryButton_;
};