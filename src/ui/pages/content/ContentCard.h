#pragma once

#include "../../components/entity/EntityCard.h"
#include "../../../domain/models/Content.h"

class LocalizationService;

class ContentCard final : public EntityCard
{
    Q_OBJECT

public:
    explicit ContentCard(
        const Content &content,
        const QString &contentTypeName,
        LocalizationService &localization,
        QWidget *parent = nullptr
    );

private:
    Content content_;
};