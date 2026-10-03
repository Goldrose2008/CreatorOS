#pragma once

#include <QWidget>

class QLabel;
class QHBoxLayout;
class QVBoxLayout;

class EntityHeader final : public QWidget
{
    Q_OBJECT

public:
    explicit EntityHeader(QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setDescription(const QString &description);
    void setStatusWidget(QWidget *widget);

    void addMetaWidget(QWidget *widget);
    void addAction(QWidget *widget);

private:
    QLabel *titleLabel_;
    QLabel *descriptionLabel_;

    QHBoxLayout *titleLayout_;
    QHBoxLayout *metaLayout_;
    QHBoxLayout *actionsLayout_;
    QVBoxLayout *textLayout_;
    QHBoxLayout *layout_;

    QWidget *statusWidget_;
};