#pragma once

#include <QWidget>

class QLabel;
class QHBoxLayout;

class PageHeader final : public QWidget
{
    Q_OBJECT

public:
    explicit PageHeader(QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setDescription(const QString &description);
    void addAction(QWidget *widget);

private:
    QLabel *titleLabel_;
    QLabel *descriptionLabel_;
    QHBoxLayout *layout_;
};