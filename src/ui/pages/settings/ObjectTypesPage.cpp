#include "ObjectTypesPage.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <exception>

#include "ObjectTypeEditorDialog.h"
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

    auto *createButton = new QPushButton(localization_.text(QStringLiteral("common.action.create")), this);
    header->addAction(createButton);

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

    connect(
        createButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            ObjectTypeEditorDialog dialog(objectTypeService_, localization_, this);
            if (dialog.exec() == QDialog::Accepted)
                reload();
        }
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

                auto *editButton = new QPushButton(localization_.text(QStringLiteral("common.action.edit")), item);
                item->addAction(editButton);

                connect(
                    editButton,
                    &QPushButton::clicked,
                    this,
                    [this, objectType]()
                    {
                        ObjectTypeEditorDialog dialog(objectTypeService_, localization_, this, objectType);
                        if (dialog.exec() == QDialog::Accepted)
                            reload();
                    }
                );

                return item;
            }
        );
    }
    catch (const std::exception &)
    {
        objectTypesList_->showError();
    }
}