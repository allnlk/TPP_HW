#pragma once

#include <memory>
#include <stdexcept>
#include <string>

#include "EventState.h"

// Додатковий патерн: Factory Method (параметризований фабричний метод).
// Стан події зберігається в БД як рядок; фабрика створює відповідний
// об'єкт стану, тож код бізнес-логіки не залежить від конкретних класів станів.
class EventStateFactory
{
public:
    static std::unique_ptr<EventState> create(const std::string& status)
    {
        if (status == "Registered") return std::make_unique<RegisteredState>();
        if (status == "Verified")   return std::make_unique<VerifiedState>();
        if (status == "Processed")  return std::make_unique<ProcessedState>();
        if (status == "Rejected")   return std::make_unique<RejectedState>();

        throw std::invalid_argument("Unknown event status: " + status);
    }
};
