#pragma once

#include <QWidget>

#include "../../../domain/models/Project.h"

class EntityCard;

class ProjectCard final : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectCard(const Project &project, class LocalizationService &localization, QWidget *parent = nullptr);

signals:
    void openRequested(std::int64_t projectId);

private:
    Project project_;
    EntityCard *card_;
};