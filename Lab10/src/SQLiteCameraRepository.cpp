#include "SQLiteCameraRepository.h"

SQLiteCameraRepository::SQLiteCameraRepository(DbStorage& storage)
    : storage_(storage)
{
}

int SQLiteCameraRepository::add(const Camera& camera)
{
    return storage_.insert(camera);
}

std::optional<Camera> SQLiteCameraRepository::findById(int id)
{
    return storage_.get_optional<Camera>(id);
}

std::vector<Camera> SQLiteCameraRepository::findAll()
{
    return storage_.get_all<Camera>();
}

bool SQLiteCameraRepository::update(const Camera& camera)
{
    auto existing = storage_.get_optional<Camera>(camera.id);
    if (!existing)
    {
        return false;
    }

    storage_.update(camera);
    return true;
}

bool SQLiteCameraRepository::remove(int id)
{
    auto existing = storage_.get_optional<Camera>(id);
    if (!existing)
    {
        return false;
    }

    // Якщо у камери є події, SQLite відхилить видалення (FOREIGN KEY)
    // і буде згенеровано виняток.
    storage_.remove<Camera>(id);
    return true;
}
