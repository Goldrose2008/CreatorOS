#pragma once

#include <QList>
#include <QString>
#include <QScrollArea>

class QVBoxLayout;
class QWidget;
class QGridLayout;

struct EntityMetadata
{
    QString label;
    QString value;
};

class EntityDetails : public QScrollArea
{
    Q_OBJECT

public:
    explicit EntityDetails(QWidget *parent = nullptr);

    void setMetadata(QGridLayout *layout, const QList<EntityMetadata> &metadata);
    
    void setNavigationWidget(QWidget *widget);
    void setHeaderWidget(QWidget *widget);
    void setStateWidget(QWidget *widget);
    void setSummaryWidget(QWidget *widget);

    void setNavigationVisible(bool visible);
    void setHeaderVisible(bool visible);
    void setStateVisible(bool visible);
    void setSummaryVisible(bool visible);

    void addContentWidget(QWidget *widget);

    QVBoxLayout *contentLayout() const;

private:
    QWidget *contentWidget_;

    QVBoxLayout *layout_;

    QVBoxLayout *navigationLayout_;
    QVBoxLayout *headerLayout_;
    QVBoxLayout *stateLayout_;
    QVBoxLayout *summaryLayout_;
    QVBoxLayout *contentLayout_;

    QWidget *navigationWidget_;
    QWidget *headerWidget_;
    QWidget *stateWidget_;
    QWidget *summaryWidget_;
};