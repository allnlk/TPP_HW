#include <filesystem>
#include <iomanip>
#include <iostream>

#include "Database.h"
#include "Demos.h"
#include "EventStore.h"
#include "ICameraRepository.h"
#include "Printing.h"
#include "SQLiteCameraRepository.h"

namespace
{
// Функція працює лише з інтерфейсом і не знає про sqlite_orm.
void printAllCameras(ICameraRepository& repository)
{
    for (const auto& camera : repository.findAll())
    {
        printCamera(camera);
    }
}
}

void runRepositoryDemo()
{
    std::cout << "\n=== TASK 4: REPOSITORY ===\n";

    std::filesystem::remove(kRepositoryDb);

    auto storage = createStorage(kRepositoryDb);
    configureStorage(storage);

    SQLiteCameraRepository sqliteRepository(storage);
    ICameraRepository& repository = sqliteRepository;
    EventStore events(storage);

    // 1. CREATE
    std::cout << "\nCREATE\n";
    Camera camera{0, "Test Cam", "Corridor", "online"};
    int id = repository.add(camera);
    std::cout << "Inserted camera ID: " << id << '\n';

    // 2. READ
    std::cout << "\nREAD\n";
    auto found = repository.findById(id);
    if (found)
    {
        printCamera(*found);
    }

    // 3. UPDATE
    std::cout << "\nUPDATE\n";
    if (found)
    {
        found->status = "maintenance";
        bool updated = repository.update(*found);
        std::cout << "Updated: " << std::boolalpha << updated << '\n';

        auto afterUpdate = repository.findById(id);
        if (afterUpdate)
        {
            std::cout << "New status: " << afterUpdate->status << '\n';
        }
    }

    // 4. READ ALL
    std::cout << "\nREAD ALL\n";
    repository.add(Camera{0, "Second Cam", "Garage", "online"});
    printAllCameras(repository);

    // 5. Відсутній запис
    std::cout << "\nMISSING RECORD (ID 9999)\n";
    std::cout << "findById has value: " << std::boolalpha
              << repository.findById(9999).has_value() << '\n';
    Camera missing{9999, "Ghost", "Nowhere", "offline"};
    std::cout << "update result: " << repository.update(missing) << '\n';
    std::cout << "remove result: " << repository.remove(9999) << '\n';

    // 6. Видалення камери, що має події (FOREIGN KEY)
    std::cout << "\nDELETE CAMERA WITH EVENTS\n";
    events.add(RegisteredEvent{0, id, "2026-11-01 12:00:00", "motion", 2, 30});
    std::cout << "Events of camera " << id << ": "
              << events.findByCamera(id).size() << '\n';
    try
    {
        repository.remove(id);
        std::cout << "Unexpected: camera removed\n";
    }
    catch (const std::exception& ex)
    {
        std::cout << "Removal rejected: " << ex.what() << '\n';
    }

    // 7. DELETE: спочатку події, потім камера
    std::cout << "\nDELETE\n";
    events.removeByCamera(id);
    bool removed = repository.remove(id);
    std::cout << "Removed: " << std::boolalpha << removed << '\n';
    std::cout << "Record exists after deletion: "
              << repository.findById(id).has_value() << '\n';

    std::cout << "\nCameras left:\n";
    printAllCameras(repository);
}
