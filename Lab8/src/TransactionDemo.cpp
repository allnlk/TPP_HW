#include <iostream>
#include <string>

#include "Database.h"
#include "Demos.h"
#include "EventStore.h"
#include "SQLiteCameraRepository.h"
#include "TestData.h"

namespace
{
struct Counts
{
    int cameras;
    int events;
};

Counts countRecords(DbStorage& storage)
{
    return {storage.count<Camera>(), storage.count<RegisteredEvent>()};
}

void printCounts(const char* label, const Counts& counts)
{
    std::cout << label << " -> Camera: " << counts.cameras
              << ", RegisteredEvent: " << counts.events << '\n';
}

bool cameraExists(DbStorage& storage, const std::string& name)
{
    using namespace sqlite_orm;
    return !storage.get_all<Camera>(where(c(&Camera::name) == name)).empty();
}
}

// Змістовна операція предметної області:
// реєстрація нової камери разом з першою подією "camera_online".
void runTransactionDemo()
{
    std::cout << "\n=== TASK 6: TRANSACTIONS ===\n\n";
    prepareTestData(kTransactionsDb);

    auto storage = createStorage(kTransactionsDb);
    configureStorage(storage);

    SQLiteCameraRepository cameras(storage);
    EventStore events(storage);

    // ---------- Сценарій 1: COMMIT ----------
    std::cout << "\n--- Scenario 1: successful transaction (COMMIT) ---\n";
    printCounts("Before", countRecords(storage));

    bool committed = storage.transaction([&]()
    {
        int cameraId = cameras.add(Camera{0, "Garage Cam", "Garage", "online"});
        std::cout << "Camera inserted inside transaction, ID: " << cameraId << '\n';

        events.add(RegisteredEvent{
            0, cameraId, "2026-11-01 09:00:00", "camera_online", 1, 0});
        std::cout << "First event inserted inside transaction\n";
        return true;
    });

    std::cout << "Transaction committed: " << std::boolalpha << committed << '\n';
    printCounts("After ", countRecords(storage));
    std::cout << "Garage Cam exists: "
              << cameraExists(storage, "Garage Cam") << '\n';

    // ---------- Сценарій 2: виняток -> ROLLBACK ----------
    std::cout << "\n--- Scenario 2: second operation fails (ROLLBACK) ---\n";
    printCounts("Before", countRecords(storage));

    try
    {
        storage.transaction([&]()
        {
            int cameraId = cameras.add(
                Camera{0, "Backyard Cam", "Backyard", "online"});
            std::cout << "Camera inserted inside transaction, ID: " << cameraId << '\n';

            // Навмисна помилка: камери з ID -1 не існує
            events.add(RegisteredEvent{
                0, -1, "2026-11-01 09:05:00", "camera_online", 1, 0});
            return true;
        });
        std::cout << "Unexpected: transaction committed\n";
    }
    catch (const std::exception& ex)
    {
        std::cout << "Transaction failed: " << ex.what() << '\n';
        std::cout << "Changes rolled back\n";
    }

    printCounts("After ", countRecords(storage));
    std::cout << "Backyard Cam exists: "
              << cameraExists(storage, "Backyard Cam") << '\n';

    // ---------- Сценарій 3: функція повернула false -> ROLLBACK ----------
    std::cout << "\n--- Scenario 3: function returns false (ROLLBACK) ---\n";
    printCounts("Before", countRecords(storage));

    bool result = storage.transaction([&]()
    {
        int cameraId = cameras.add(
            Camera{0, "Basement Cam", "Basement", "online"});
        events.add(RegisteredEvent{
            0, cameraId, "2026-11-01 09:10:00", "camera_online", 1, 0});
        std::cout << "Both records inserted, but registration is cancelled\n";
        return false;
    });

    std::cout << "Transaction committed: " << result << '\n';
    printCounts("After ", countRecords(storage));
    std::cout << "Basement Cam exists: "
              << cameraExists(storage, "Basement Cam") << '\n';

    // ---------- Сценарій 4: без явної транзакції ----------
    std::cout << "\n--- Scenario 4: the same actions WITHOUT transaction ---\n";
    printCounts("Before", countRecords(storage));

    try
    {
        int cameraId = cameras.add(Camera{0, "Gate Cam", "Gate", "online"});
        std::cout << "Camera inserted, ID: " << cameraId << '\n';

        events.add(RegisteredEvent{
            0, -1, "2026-11-01 09:15:00", "camera_online", 1, 0});
    }
    catch (const std::exception& ex)
    {
        std::cout << "Operation failed: " << ex.what() << '\n';
    }

    printCounts("After ", countRecords(storage));
    std::cout << "Gate Cam exists (partial result): "
              << cameraExists(storage, "Gate Cam") << '\n';
}
