#pragma once

#include <optional>
#include <vector>

#include "Database.h"
#include "RegisteredEvent.h"

// Окремий компонент доступу до залежної сутності RegisteredEvent
// (абстрактний інтерфейс для нього не потрібен).
class EventStore
{
public:
    explicit EventStore(DbStorage& storage);

    int add(const RegisteredEvent& event);
    std::optional<RegisteredEvent> findById(int id);
    std::vector<RegisteredEvent> findAll();
    std::vector<RegisteredEvent> findByCamera(int cameraId);
    void removeByCamera(int cameraId);

private:
    DbStorage& storage_;
};
