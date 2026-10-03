#pragma once

#include <QWidget>

class QLabel;
class QVBoxLayout;

class StateWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StateWidget(QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setDescription(const QString &description);

protected:
    QVBoxLayout *contentLayout() const;

private:
    QLabel *titleLabel_;
    QLabel *descriptionLabel_;
    QVBoxLayout *layout_;
};