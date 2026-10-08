#pragma once

#include "../../components/entity/EntityCard.h"
#include "../../../domain/models/Task.h"

class LocalizationService;

class TaskCard final : public EntityCard
{
    Q_OBJECT

public:
    explicit TaskCard(const Task &task, LocalizationService &localization, QWidget *parent = nullptr);

signals:
    void openRequested(std::int64_t taskId);

private:
    Task task_;
};