#pragma once

#include <QSqlDatabase>

#include "../../application/projects/IProjectRepository.h"

class DatabaseManager;

class ProjectRepository final : public IProjectRepository
{
public:
    explicit ProjectRepository(const DatabaseManager &databaseManager);

    std::vector<Project> findAll() const override;
    std::optional<Project> findById(std::int64_t id) const override;
    std::int64_t createWithMainContent(const Project &project, const Content &mainContent) override;

    bool update(const Project &project) override;
    bool remove(std::int64_t id) override;

private:
    QSqlDatabase database_;
};