#include "TestData.h"

#include <filesystem>
#include <iostream>
#include <vector>

#include "Database.h"
#include "EventStore.h"
#include "SQLiteCameraRepository.h"

namespace
{
struct EventSeed
{
    int cameraIndex;          // індекс камери у списку (0..4)
    const char* eventTime;
    const char* eventType;
    int severity;
    int durationSec;
};
}

void prepareTestData(const std::string& databaseFile)
{
    // Видаляється лише окрема демонстраційна база.
    std::filesystem::remove(databaseFile);

    auto storage = createStorage(databaseFile);
    configureStorage(storage);

    SQLiteCameraRepository cameras(storage);
    EventStore events(storage);

    const std::vector<Camera> cameraList = {
        {0, "Entrance Cam",  "Main entrance", "online"},
        {0, "Parking Cam A", "Parking lot",   "online"},
        {0, "Lobby Cam",     "Lobby",         "maintenance"},
        {0, "Warehouse Cam", "Warehouse",     "online"},
        {0, "Roof Cam",      "Roof",          "offline"}
    };

    std::vector<int> cameraIds;
    for (const auto& camera : cameraList)
    {
        cameraIds.push_back(cameras.add(camera));
    }

    const std::vector<EventSeed> seeds = {
        {0, "2026-10-01 08:15:00", "motion",          2,  30},
        {0, "2026-10-05 22:40:00", "intrusion",       5, 120},
        {0, "2026-10-12 13:05:00", "person_detected", 3,  45},

        {1, "2026-10-02 09:30:00", "vehicle_detected", 1, 20},
        {1, "2026-10-07 23:10:00", "motion",           3, 60},
        {1, "2026-10-15 18:20:00", "vehicle_detected", 2, 25},

        {2, "2026-09-20 11:00:00", "camera_offline",  4, 600},
        {2, "2026-10-03 14:45:00", "motion",          2,  40},
        {2, "2026-10-18 07:55:00", "person_detected", 3,  50},

        {3, "2026-10-04 02:10:00", "intrusion",       5, 180},
        {3, "2026-10-09 03:25:00", "motion",          4,  90},
        {3, "2026-10-20 01:40:00", "motion",          3,  70},

        {4, "2026-09-25 16:00:00", "camera_offline",  4, 900},
        {4, "2026-10-11 05:30:00", "motion",          1,  15},
        {4, "2026-10-22 19:50:00", "person_detected", 2,  35}
    };

    for (const auto& seed : seeds)
    {
        RegisteredEvent event{
            0,
            cameraIds[seed.cameraIndex],
            seed.eventTime,
            seed.eventType,
            seed.severity,
            seed.durationSec
        };
        events.add(event);
    }

    std::cout << "Test database prepared: " << databaseFile << '\n';
    std::cout << "Camera records: " << storage.count<Camera>() << '\n';
    std::cout << "RegisteredEvent records: "
              << storage.count<RegisteredEvent>() << '\n';
}
