#pragma once

#include <QWidget>

class QLabel;
class QVBoxLayout;

class Section final : public QWidget
{
    Q_OBJECT

public:
    explicit Section(QWidget *parent = nullptr);

    void setTitle(const QString &title);

    QVBoxLayout *contentLayout() const;

private:
    QLabel *titleLabel_;
    QVBoxLayout *layout_;
};