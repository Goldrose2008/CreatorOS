#pragma once

#include <QProxyStyle>

class CreatorStyle final : public QProxyStyle
{
public:
    CreatorStyle();

    void polish(QPalette &palette) override;
};