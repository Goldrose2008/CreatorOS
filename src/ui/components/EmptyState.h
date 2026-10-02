#pragma once

#include <QWidget>

class QLabel;

class EmptyState final : public QWidget
{
    Q_OBJECT

public:
    explicit EmptyState(QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setDescription(const QString &description);

private:
    QLabel *titleLabel_;
    QLabel *descriptionLabel_;
};