#pragma once

#include <QWidget>

class QScrollArea;
class QStackedWidget;
class QVBoxLayout;
class QWidget;

class EmptyState;
class ErrorState;
class LoadingState;
class LocalizationService;

class EntityList final : public QWidget
{
    Q_OBJECT

public:
    explicit EntityList(LocalizationService &localization, QWidget *parent = nullptr);

    void addItem(QWidget *item);
    void clearItems();
    int count() const;

    void setLoadingTitle(const QString &title = QString());
    void setEmptyState(const QString &title = QString(), const QString &description = QString());
    void setErrorState(const QString &title = QString(), const QString &description = QString(), const QString &retryText = QString());

    void showLoading();
    void showList();
    void showEmpty();
    void showError();

signals:
    void retryRequested();

private:
    QScrollArea *scrollArea_;
    QWidget *contentWidget_;
    QVBoxLayout *layout_;

    QStackedWidget *stateStack_;
    LoadingState *loadingState_;
    EmptyState *emptyState_;
    ErrorState *errorState_;

    LocalizationService &localization_;

    int itemCount_;
};