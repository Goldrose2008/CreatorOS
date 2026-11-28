#pragma once

#include <QWidget>

class LocalizationService;

class SettingsPage final : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPage(LocalizationService &localization, QWidget *parent = nullptr);

signals:
    void objectTypesRequested();
};