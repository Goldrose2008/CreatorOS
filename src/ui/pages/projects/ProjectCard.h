#pragma once

#include "../../../domain/models/Project.h"
#include "../../../ui/components/entity/EntityCard.h"

class LocalizationService;

class ProjectCard final : public EntityCard
{
    Q_OBJECT

public:
    explicit ProjectCard(const Project &project, class LocalizationService &localization, QWidget *parent = nullptr);

signals:
    void openRequested(std::int64_t projectId);

private:
    Project project_;
};