#pragma once

#include <QWidget>

class QScrollArea;
class QStackedWidget;
class QVBoxLayout;
class QWidget;

class EmptyState;
class ErrorState;
class LoadingState;

class EntityList final : public QWidget
{
    Q_OBJECT

public:
    explicit EntityList(QWidget *parent = nullptr);

    void addItem(QWidget *item);
    void clearItems();
    int count() const;

    void setLoadingTitle(const QString &title);
    void setEmptyState(const QString &title, const QString &description);
    void setErrorState(const QString &title, const QString &description, const QString &retryText);

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

    int itemCount_;
};