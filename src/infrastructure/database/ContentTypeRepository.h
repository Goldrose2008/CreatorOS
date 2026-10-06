#pragma once

#include <QSqlDatabase>

#include "../../application/content/IContentTypeRepository.h"

class DatabaseManager;

class ContentTypeRepository final : public IContentTypeRepository
{
public:
    explicit ContentTypeRepository(const DatabaseManager &databaseManager);

    std::vector<ContentType> findAll() const override;
    std::optional<ContentType> findById(std::int64_t id) const override;

private:
    QSqlDatabase database_;
};