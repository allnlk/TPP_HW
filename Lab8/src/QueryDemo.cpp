#include <iomanip>
#include <iostream>

#include "Database.h"
#include "Demos.h"
#include "Printing.h"

void runQueryDemo()
{
    using namespace sqlite_orm;

    std::cout << "\n=== TASK 5: FILTERING, SORTING, LIMIT ===\n";

    auto storage = createStorage(kQueriesDb);
    configureStorage(storage);

    // 1. Фільтрація за одним полем
    std::cout << "\n1. Online cameras:\n";
    auto onlineCameras = storage.get_all<Camera>(
        where(c(&Camera::status) == "online")
    );
    for (const auto& camera : onlineCameras)
    {
        printCamera(camera);
    }

    // 2. Фільтрація за двома полями
    std::cout << "\n2. Motion events with severity >= 3:\n";
    auto motionEvents = storage.get_all<RegisteredEvent>(
        where(c(&RegisteredEvent::eventType) == "motion"
              && c(&RegisteredEvent::severity) >= 3)
    );
    for (const auto& event : motionEvents)
    {
        printEvent(event);
    }

    // 3. Сортування
    std::cout << "\n3. Cameras sorted by name:\n";
    auto sortedCameras = storage.get_all<Camera>(
        order_by(&Camera::name).asc()
    );
    for (const auto& camera : sortedCameras)
    {
        printCamera(camera);
    }

    // 4. Сортування + обмеження кількості
    std::cout << "\n4. Top 3 most severe events:\n";
    auto topEvents = storage.get_all<RegisteredEvent>(
        multi_order_by(
            order_by(&RegisteredEvent::severity).desc(),
            order_by(&RegisteredEvent::durationSec).desc()
        ),
        limit(3)
    );
    for (const auto& event : topEvents)
    {
        printEvent(event);
    }
}

void runAggregateDemo()
{
    using namespace sqlite_orm;

    std::cout << "\n=== TASK 5: AGGREGATION ===\n";

    auto storage = createStorage(kQueriesDb);
    configureStorage(storage);

    std::cout << std::fixed << std::setprecision(2);

    // COUNT
    std::cout << "\n1. Events count: "
              << storage.count<RegisteredEvent>() << '\n';
    std::cout << "2. Events with severity >= 4: "
              << storage.count<RegisteredEvent>(
                     where(c(&RegisteredEvent::severity) >= 4))
              << '\n';

    // AVG
    std::cout << "3. Average severity: "
              << storage.avg(&RegisteredEvent::severity) << '\n';
    std::cout << "4. Average duration (s): "
              << storage.avg(&RegisteredEvent::durationSec) << '\n';

    // GROUP BY
    std::cout << "\n5. Statistics by camera:\n";
    auto stats = storage.select(
        columns(
            &RegisteredEvent::cameraId,
            count(&RegisteredEvent::id),
            avg(&RegisteredEvent::severity),
            avg(&RegisteredEvent::durationSec)
        ),
        group_by(&RegisteredEvent::cameraId)
    );
    for (const auto& [cameraId, eventCount, avgSeverity, avgDuration] : stats)
    {
        std::cout << "Camera ID: " << cameraId
                  << " | events: " << eventCount
                  << " | avg severity: " << avgSeverity
                  << " | avg duration: " << avgDuration << '\n';
    }
}

void runJoinDemo()
{
    using namespace sqlite_orm;

    std::cout << "\n=== TASK 5: INNER JOIN ===\n";

    auto storage = createStorage(kQueriesDb);
    configureStorage(storage);

    // 1. Усі події з назвою та місцем розташування камери
    std::cout << "\n1. Events with camera data:\n";
    auto records = storage.select(
        columns(
            &Camera::name,
            &Camera::location,
            &RegisteredEvent::eventTime,
            &RegisteredEvent::eventType,
            &RegisteredEvent::severity
        ),
        inner_join<RegisteredEvent>(
            on(c(&Camera::id) == &RegisteredEvent::cameraId)
        ),
        order_by(&RegisteredEvent::id)
    );
    for (const auto& [name, location, time, type, severity] : records)
    {
        std::cout << name << " | " << location << " | "
                  << time << " | " << type << " | "
                  << severity << '\n';
    }

    // 2. JOIN + фільтрація + сортування
    std::cout << "\n2. Events longer than 100 s:\n";
    auto longEvents = storage.select(
        columns(
            &Camera::name,
            &RegisteredEvent::eventType,
            &RegisteredEvent::durationSec
        ),
        inner_join<RegisteredEvent>(
            on(c(&Camera::id) == &RegisteredEvent::cameraId)
        ),
        where(c(&RegisteredEvent::durationSec) > 100),
        order_by(&RegisteredEvent::durationSec).desc()
    );
    for (const auto& [name, type, duration] : longEvents)
    {
        std::cout << name << " | " << type << " | "
                  << duration << " s\n";
    }
}
