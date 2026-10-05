#pragma once

#include "../layout/Card.h"

class QLabel;
class QHBoxLayout;
class QVBoxLayout;
class QProgressBar;

class EntityCard : public Card
{
    Q_OBJECT

public:
    explicit EntityCard(QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setDescription(const QString &description);
    void setStatusWidget(QWidget *widget);

    void addContentWidget(QWidget *widget);
    void addMetaWidget(QWidget *widget);

    void setProgress(int value);
    void clearProgress();

    void addAction(QWidget *widget);

private:
    QLabel *titleLabel_;
    QLabel *descriptionLabel_;

    QHBoxLayout *titleLayout_;
    QHBoxLayout *metaLayout_;
    QHBoxLayout *actionsLayout_;
    QVBoxLayout *contentWidgetsLayout_;

    QProgressBar *progressBar_;

    QWidget *statusWidget_;
};