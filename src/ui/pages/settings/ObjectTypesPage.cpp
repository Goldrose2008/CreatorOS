#include "ObjectTypesPage.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <exception>

#include "../../components/entity/EntityCard.h"
#include "../../components/entity/EntityList.h"
#include "../../components/layout/PageHeader.h"
#include "../../localization/LocalizationService.h"
#include "../../style/Metrics.h"
#include "../../../application/object_types/ObjectTypeService.h"
#include "../../../domain/models/ObjectType.h"

ObjectTypesPage::ObjectTypesPage(ObjectTypeService &objectTypeService, LocalizationService &localization, QWidget *parent)
    : QWidget(parent),
      objectTypeService_(objectTypeService),
      localization_(localization),
      objectTypesList_(new EntityList(localization_, this))
{
    auto *layout = new QVBoxLayout(this);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(CreatorMetrics::SpacingXLarge);

    auto *header = new PageHeader(this);
    header->setTitle(localization_.text(QStringLiteral("object_types")));
    header->setDescription(localization_.text(QStringLiteral("object_types.description")));

    auto *backButton = new QPushButton(localization_.text(QStringLiteral("common.action.back")), this);
    header->addAction(backButton);
    objectTypesList_->setEmptyState(QString(), localization_.text(QStringLiteral("object_types.empty.description")));
    layout->addWidget(header);
    layout->addWidget(objectTypesList_, 1);

    connect(
        backButton,
        &QPushButton::clicked,
        this,
        &ObjectTypesPage::backRequested
    );

    connect(
        objectTypesList_,
        &EntityList::retryRequested,
        this,
        &ObjectTypesPage::reload
    );

    reload();
}

void ObjectTypesPage::reload()
{
    objectTypesList_->showLoading();

    try
    {
        const std::vector<ObjectType> objectTypes = objectTypeService_.getAllObjectTypes();

        objectTypesList_->setItems(
            objectTypes,
            [this](const ObjectType &objectType) -> QWidget *
            {
                auto *item = new EntityCard(localization_, objectTypesList_);
                item->setTitle(QString::fromUtf8(objectType.name.c_str()));

                if (objectType.description.has_value())
                    item->setDescription(QString::fromUtf8(objectType.description->c_str()));

                return item;
            }
        );
    }
    catch (const std::exception &)
    {
        objectTypesList_->showError();
    }
}