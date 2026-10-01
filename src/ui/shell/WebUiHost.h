#pragma once

#include <QWidget>

class QWebChannel;
class QWebEngineView;
class WebBridge;

class WebUiHost final : public QWidget
{
    Q_OBJECT

public:
    explicit WebUiHost(QWidget *parent = nullptr);

private:
    QWebEngineView *webView_;
    QWebChannel *webChannel_;
    WebBridge *webBridge_;
};