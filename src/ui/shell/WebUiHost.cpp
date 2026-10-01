#include "WebUiHost.h"

#include <QVBoxLayout>
#include <QWebChannel>
#include <QWebEngineView>
#include <QWebEnginePage>

#include "../bridge/WebBridge.h"

WebUiHost::WebUiHost(QWidget *parent)
    : QWidget(parent),
      webView_(new QWebEngineView(this)),
      webChannel_(new QWebChannel(this)),
      webBridge_(new WebBridge(this))
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(webView_);

    webChannel_->registerObject("bridge", webBridge_);
    webView_->page()->setWebChannel(webChannel_);

    webView_->setUrl(QUrl("qrc:///ui/index.html"));
}