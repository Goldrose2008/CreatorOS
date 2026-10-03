#pragma once

#include <QAbstractButton>
#include <QIcon>

class MenuItem final : public QAbstractButton
{
    Q_OBJECT

public:
    explicit MenuItem(QWidget *parent = nullptr);

    void setIcon(const QIcon &icon);
    void setRoute(const QString &route);
    void setActive(bool active);

    QString route() const;

signals:
    void triggered(const QString &route);

protected:
    void paintEvent(QPaintEvent *event) override;
    QSize sizeHint() const override;

private:
    QIcon icon_;
    QString route_;
};