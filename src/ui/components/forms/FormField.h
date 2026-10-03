#pragma once

#include <QWidget>

class QLabel;
class QVBoxLayout;

class FormField final : public QWidget
{
    Q_OBJECT

public:
    explicit FormField(QWidget *parent = nullptr);

    void setLabel(const QString &label);
    void setDescription(const QString &description);
    void setError(const QString &error);

    void setField(QWidget *field);

private:
    QLabel *label_;
    QLabel *descriptionLabel_;
    QLabel *errorLabel_;
    QVBoxLayout *layout_;
};