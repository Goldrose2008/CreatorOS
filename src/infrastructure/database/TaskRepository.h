#pragma once

#include <QSqlDatabase>

#include "../../application/tasks/ITaskRepository.h"

class DatabaseManager;

class TaskRepository final : public ITaskRepository
{
public:
    explicit TaskRepository(const DatabaseManager &databaseManager);

    std::vector<Task> findAll() const override;
    std::vector<Task> findByParent(TaskParentType parentType, std::int64_t parentId, const std::optional<std::int64_t> &parentTaskId) const override;
    std::optional<Task> findById(std::int64_t id) const override;
    std::int64_t create(const Task &task) override;
    bool update(const Task &task) override;
    bool remove(std::int64_t id) override;

private:
    QSqlDatabase database_;
};