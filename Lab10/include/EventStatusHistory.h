#pragma once

#include <string>

// Lab10: запис історії зміни стану події.
struct EventStatusHistory
{
    int id = 0;
    int eventId = 0;           // зовнішній ключ -> RegisteredEvent(id)
    std::string oldStatus;
    std::string newStatus;
    std::string changedAt;     // формат YYYY-MM-DD HH:MM:SS
};
