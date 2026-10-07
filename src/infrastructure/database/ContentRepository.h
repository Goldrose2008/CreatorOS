#pragma once

#include <QSqlDatabase>

#include "../../application/content/IContentRepository.h"

class DatabaseManager;

class ContentRepository final : public IContentRepository
{
public:
    explicit ContentRepository(const DatabaseManager &databaseManager);

    std::vector<Content> findByProjectId(std::int64_t projectId) const override;
    std::optional<Content> findById(std::int64_t id) const override;

    std::int64_t create(const Content &content) override;
    bool update(const Content &content) override;
    bool remove(std::int64_t id) override;

private:
    QSqlDatabase database_;
};