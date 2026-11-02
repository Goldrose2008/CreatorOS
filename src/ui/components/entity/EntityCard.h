#pragma once

#include <functional>

#include "../layout/Card.h"

class QLabel;
class QHBoxLayout;
class QVBoxLayout;
class QProgressBar;
class LocalizationService;

class EntityCard : public Card
{
    Q_OBJECT

public:
    explicit EntityCard(LocalizationService &localization, QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setDescription(const QString &description);
    void setDeadline(const QString &label, const QString &value);
    void setProgress(int value);
    void clearProgress();

    void addContentWidget(QWidget *widget);
    void addMetaWidget(QWidget *widget);
    void addAction(QWidget *widget);
    void setOpenAction(std::function<void()> handler, const QString &text = QString());
        
private:
    LocalizationService &localization_;
    QLabel *titleLabel_;
    QLabel *descriptionLabel_;
    QHBoxLayout *titleLayout_;
    QHBoxLayout *metaLayout_;
    QHBoxLayout *actionsLayout_;
    QVBoxLayout *contentWidgetsLayout_;
    QProgressBar *progressBar_;
};