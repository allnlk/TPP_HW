#pragma once

#include <optional>
#include <vector>

#include "Camera.h"

// Інтерфейс Repository для основної сутності Camera.
class ICameraRepository
{
public:
    virtual ~ICameraRepository() = default;

    // Create
    virtual int add(const Camera& camera) = 0;

    // Read
    virtual std::optional<Camera> findById(int id) = 0;
    virtual std::vector<Camera> findAll() = 0;

    // Update: true - запис існував і оновлений, false - запис не знайдено
    virtual bool update(const Camera& camera) = 0;

    // Delete: true - запис видалено, false - запис не знайдено
    virtual bool remove(int id) = 0;
};
