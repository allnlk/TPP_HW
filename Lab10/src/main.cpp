#include <exception>
#include <iomanip>
#include <iostream>
#include <memory>

#include "Database.h"
#include "EventObserver.h"
#include "EventProcessingService.h"
#include "EventStore.h"
#include "PriorityStrategy.h"
#include "StatusHistoryStore.h"
#include "TestData.h"

namespace
{
const std::string kDb = "surveillance_lab10.db";

void tryAction(const char* label, const std::function<std::string()>& action)
{
    try
    {
        std::string result = action();
        std::cout << label << " -> " << result << '\n';
    }
    catch (const std::exception& ex)
    {
        std::cout << label << " -> rejected: " << ex.what() << '\n';
    }
}

void printHistory(StatusHistoryStore& history, int eventId)
{
    std::cout << "History of event " << eventId << ":\n";
    for (const auto& record : history.findByEvent(eventId))
    {
        std::cout << "  " << record.changedAt << " | "
                  << record.oldStatus << " -> " << record.newStatus << '\n';
    }
}
}

int main()
{
    try
    {
        prepareTestData(kDb);

        auto storage = createStorage(kDb);
        configureStorage(storage);

        EventStore events(storage);
        StatusHistoryStore history(storage);
        EventPublisher publisher;
        EventProcessingService service(storage, publisher);

        // ---------- Observer ----------
        std::cout << "\n=== OBSERVER: subscribe ===\n";
        auto cameraLog = std::make_shared<CameraLog>();
        auto alerts = std::make_shared<AlertSystem>();
        publisher.subscribe(cameraLog);
        publisher.subscribe(alerts);

        int intrusionId = service.registerEvent(
            RegisteredEvent{0, 1, "2026-11-23 02:15:00", "intrusion", 5, 240});
        int motionId = service.registerEvent(
            RegisteredEvent{0, 2, "2026-11-23 02:20:00", "motion", 2, 30});

        // ---------- State ----------
        std::cout << "\n=== STATE: allowed transitions ===\n";
        tryAction("verify intrusion ", [&] { return service.verify(intrusionId); });
        tryAction("process intrusion", [&] { return service.process(intrusionId); });
        printHistory(history, intrusionId);

        std::cout << "\n=== STATE: rejected event ===\n";
        tryAction("reject motion    ", [&] { return service.reject(motionId); });
        printHistory(history, motionId);

        std::cout << "\n=== STATE: invalid transitions ===\n";
        tryAction("reject processed ", [&] { return service.reject(intrusionId); });
        tryAction("verify rejected  ", [&] { return service.verify(motionId); });
        tryAction("process event 1  ", [&] { return service.process(1); });
        tryAction("verify event 999 ", [&] { return service.verify(999); });

        // ---------- Observer: unsubscribe ----------
        std::cout << "\n=== OBSERVER: AlertSystem unsubscribed ===\n";
        publisher.unsubscribe(alerts);
        tryAction("verify event 10  ", [&] { return service.verify(10); });
        std::cout << "Alerts sent: " << alerts->getAlertCount()
                  << ", camera log entries: " << cameraLog->getEntries().size() << '\n';

        // ---------- Strategy ----------
        std::cout << "\n=== STRATEGY: priority of events ===\n";
        std::cout << std::fixed << std::setprecision(3);

        EventPriorityCalculator calculator(std::make_unique<MaxNormalizedPriority>());
        const int waitingSec = 1200;  // подія очікує опрацювання 20 хв

        for (int id : {4, 5, 7})
        {
            auto event = events.findById(id);
            if (!event)
            {
                continue;
            }
            PriorityParams params = toPriorityParams(*event, waitingSec);
            std::cout << "Event " << id << " (" << event->eventType
                      << ", severity " << event->severity << ", "
                      << event->durationSec << " s, waiting "
                      << waitingSec << " s)\n";

            calculator.setStrategy(std::make_unique<MaxNormalizedPriority>());
            std::cout << "  max of normalized      : " << calculator.calculate(params) << '\n';

            calculator.setStrategy(std::make_unique<WeightedSumPriority>());
            std::cout << "  weighted sum           : " << calculator.calculate(params) << '\n';

            calculator.setStrategy(std::make_unique<NormalizedWeightedSumPriority>());
            std::cout << "  normalized weighted sum: " << calculator.calculate(params) << '\n';
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }

    return 0;
}
