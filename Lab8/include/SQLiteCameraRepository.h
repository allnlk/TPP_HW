#pragma once

#include "Database.h"
#include "ICameraRepository.h"

// Реалізація Repository через sqlite_orm.
class SQLiteCameraRepository : public ICameraRepository
{
public:
    explicit SQLiteCameraRepository(DbStorage& storage);

    int add(const Camera& camera) override;
    std::optional<Camera> findById(int id) override;
    std::vector<Camera> findAll() override;
    bool update(const Camera& camera) override;
    bool remove(int id) override;

private:
    DbStorage& storage_;
};
