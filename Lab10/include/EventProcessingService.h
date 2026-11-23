#pragma once

#include <functional>
#include <string>

#include "Database.h"
#include "EventObserver.h"
#include "EventState.h"
#include "EventStore.h"
#include "StatusHistoryStore.h"

// Бізнес-логіка обробки подій відеоспостереження.
// State - правила переходів, EventStore/StatusHistoryStore - збереження,
// EventPublisher - сповіщення підписників.
class EventProcessingService
{
public:
    using Clock = std::function<std::string()>;

    static constexpr int kCriticalSeverity = 4;

    EventProcessingService(DbStorage& storage, EventPublisher& publisher,
                           Clock clock = currentTime);

    int registerEvent(RegisteredEvent event);

    std::string verify(int eventId);
    std::string process(int eventId);
    std::string reject(int eventId);

    static std::string currentTime();

private:
    std::string changeState(int eventId,
                            const std::function<void(EventWorkflow&)>& action);

    DbStorage& storage_;
    EventStore events_;
    StatusHistoryStore history_;
    EventPublisher& publisher_;
    Clock clock_;
};
