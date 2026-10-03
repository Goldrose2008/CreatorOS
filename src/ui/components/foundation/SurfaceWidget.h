#pragma once

#include <QColor>
#include <QWidget>

class QVBoxLayout;

class SurfaceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SurfaceWidget(QWidget *parent = nullptr);

    QVBoxLayout *contentLayout() const;

protected:
    void paintEvent(QPaintEvent *event) override;

    virtual QColor fillColor() const = 0;

private:
    QVBoxLayout *layout_;
};
