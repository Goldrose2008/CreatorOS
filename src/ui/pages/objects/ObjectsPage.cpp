
#include "ObjectsPage.h"

#include <QHash>
#include <QLabel>
#include <QVBoxLayout>
#include <QtMath>

#include <exception>

#include "../../components/entity/EntityCard.h"
#include "../../components/entity/EntityList.h"
#include "../../components/layout/PageHeader.h"
#include "../../localization/LocalizationService.h"
#include "../../style/Metrics.h"
#include "../../../application/objects/ObjectService.h"
#include "../../../application/object_types/ObjectTypeService.h"
#include "../../../domain/models/Object.h"
#include "../../../domain/models/ObjectType.h"

ObjectsPage::ObjectsPage(
    ObjectService &objectService,
    ObjectTypeService &objectTypeService,
    LocalizationService &localization,
    QWidget *parent
)
    : QWidget(parent),
      objectService_(objectService),
      objectTypeService_(objectTypeService),
      localization_(localization),
      objectList_(new EntityList(localization_, this))
{
    auto *layout = new QVBoxLayout(this);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(CreatorMetrics::SpacingXLarge);

    auto *header = new PageHeader(this);
    header->setTitle(localization_.text(QStringLiteral("objects")));
    header->setDescription(localization_.text(QStringLiteral("objects.description")));

    objectList_->setEmptyState(QString(), localization_.text(QStringLiteral("objects.empty.description")));
    layout->addWidget(header);
    layout->addWidget(objectList_, 1);

    connect(
        objectList_,
        &EntityList::retryRequested,
        this,
        &ObjectsPage::reload
    );

    reload();
}

void ObjectsPage::reload()
{
    objectList_->showLoading();

    try
    {
        const std::vector<Object> objects = objectService_.getAllActiveObjects();
        const std::vector<ObjectType> objectTypes = objectTypeService_.getAllObjectTypes();

        QHash<qint64, QString> objectTypeNames;

        for (const ObjectType &objectType : objectTypes)
            objectTypeNames.insert(objectType.id, QString::fromUtf8(objectType.name.c_str()));

        objectList_->setItems(
            objects,
            [this, objectTypeNames](const Object &object) -> QWidget *
            {
                auto *item = new EntityCard(localization_, objectList_);
                item->setTitle(QString::fromUtf8(object.name.c_str()));

                if (object.objectTypeId.has_value())
                {
                    const auto typeIterator = objectTypeNames.constFind(object.objectTypeId.value());

                    if (typeIterator != objectTypeNames.cend())
                    {
                        auto *typeLabel = new QLabel(localization_.text(QStringLiteral("objects.type")) + QStringLiteral(": ") + typeIterator.value(), item);
                        item->addMetaWidget(typeLabel);
                    }
                }

                if (object.progress.has_value())
                    item->setProgress(qRound(object.progress.value()));

                if (object.deadlineAt.has_value())
                    item->setDeadline(localization_.text(QStringLiteral("objects.deadline")), QString::fromUtf8(object.deadlineAt->c_str()));

                return item;
            }
        );
    }
    catch (const std::exception &)
    {
        objectList_->showError();
    }
}
