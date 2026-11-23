#include "EventProcessingService.h"

#include <ctime>
#include <stdexcept>

#include "EventStateFactory.h"

EventProcessingService::EventProcessingService(DbStorage& storage,
                                               EventPublisher& publisher,
                                               Clock clock)
    : storage_(storage),
      events_(storage),
      history_(storage),
      publisher_(publisher),
      clock_(std::move(clock))
{
}

std::string EventProcessingService::currentTime()
{
    std::time_t now = std::time(nullptr);
    std::tm local{};
#ifdef _WIN32
    localtime_s(&local, &now);
#else
    localtime_r(&now, &local);
#endif
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &local);
    return buffer;
}

int EventProcessingService::registerEvent(RegisteredEvent event)
{
    event.status = "Registered";
    event.id = events_.add(event);

    publisher_.notify({SurveillanceEventType::EventRegistered,
                       event.id, event.cameraId, event.eventType, event.severity});
    return event.id;
}

std::string EventProcessingService::verify(int eventId)
{
    std::string newStatus = changeState(eventId,
        [](EventWorkflow& workflow) { workflow.verify(); });

    // Сповіщення надсилається лише після успішного COMMIT
    auto event = events_.findById(eventId);
    if (event && event->severity >= kCriticalSeverity)
    {
        publisher_.notify({SurveillanceEventType::CriticalEventConfirmed,
                           event->id, event->cameraId, event->eventType,
                           event->severity});
    }
    return newStatus;
}

std::string EventProcessingService::process(int eventId)
{
    return changeState(eventId,
        [](EventWorkflow& workflow) { workflow.process(); });
}

std::string EventProcessingService::reject(int eventId)
{
    return changeState(eventId,
        [](EventWorkflow& workflow) { workflow.reject(); });
}

std::string EventProcessingService::changeState(
    int eventId, const std::function<void(EventWorkflow&)>& action)
{
    auto event = events_.findById(eventId);
    if (!event)
    {
        throw std::out_of_range("Event " + std::to_string(eventId) + " not found");
    }

    // 1. Перевірка допустимості переходу (State). Недопустимий перехід
    //    генерує InvalidTransitionError, і база даних не змінюється.
    EventWorkflow workflow(EventStateFactory::create(event->status));
    action(workflow);

    const std::string oldStatus = event->status;
    const std::string newStatus = workflow.stateName();
    event->status = newStatus;

    // 2. Атомарне збереження: новий стан + запис в історії.
    //    Виняток усередині -> ROLLBACK обох операцій.
    storage_.transaction([&]()
    {
        events_.update(*event);
        history_.add(EventStatusHistory{0, eventId, oldStatus, newStatus, clock_()});
        return true;
    });

    return newStatus;
}
