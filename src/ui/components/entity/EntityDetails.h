#pragma once

#include <QScrollArea>

class QVBoxLayout;
class QWidget;

class EntityDetails final : public QScrollArea
{
    Q_OBJECT

public:
    explicit EntityDetails(QWidget *parent = nullptr);

    void setNavigationWidget(QWidget *widget);
    void setHeaderWidget(QWidget *widget);
    void setErrorWidget(QWidget *widget);
    void setSummaryWidget(QWidget *widget);

    void addContentWidget(QWidget *widget);

    QVBoxLayout *contentLayout() const;

private:
    QWidget *contentWidget_;

    QVBoxLayout *layout_;

    QVBoxLayout *navigationLayout_;
    QVBoxLayout *headerLayout_;
    QVBoxLayout *errorLayout_;
    QVBoxLayout *summaryLayout_;
    QVBoxLayout *contentLayout_;

    QWidget *navigationWidget_;
    QWidget *headerWidget_;
    QWidget *errorWidget_;
    QWidget *summaryWidget_;
};