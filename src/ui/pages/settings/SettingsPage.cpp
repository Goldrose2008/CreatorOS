#include "SettingsPage.h"

#include <QPushButton>
#include <QVBoxLayout>

#include "../../components/layout/PageHeader.h"
#include "../../localization/LocalizationService.h"
#include "../../style/Metrics.h"

SettingsPage::SettingsPage(LocalizationService &localization, QWidget *parent) : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(CreatorMetrics::SpacingXLarge);

    auto *header = new PageHeader(this);
    header->setTitle(localization.text(QStringLiteral("settings")));
    header->setDescription(localization.text(QStringLiteral("settings.description")));

    auto *objectTypesButton = new QPushButton(localization.text(QStringLiteral("object_types")), this);

    layout->addWidget(header);
    layout->addWidget(objectTypesButton, 0, Qt::AlignLeft);
    layout->addStretch();

    connect(
        objectTypesButton,
        &QPushButton::clicked,
        this,
        &SettingsPage::objectTypesRequested
    );
}