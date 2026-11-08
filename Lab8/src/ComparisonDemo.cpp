#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

#include "Database.h"
#include "Demos.h"
#include "Printing.h"
#include "RawSqliteCamera.h"

void runComparisonDemo()
{
    std::cout << "\n=== TASK 2: SQLITE C API vs SQLITE_ORM ===\n";

    // ---------- 1. SQLite C API ----------
    std::cout << "\n--- SQLite C API ---\n";
    std::filesystem::remove(kRawDb);

    sqlite3* db = nullptr;
    if (sqlite3_open(kRawDb.c_str(), &db) != SQLITE_OK)
    {
        std::string error = sqlite3_errmsg(db);
        sqlite3_close(db);
        throw std::runtime_error("Cannot open database: " + error);
    }

    try
    {
        createCameraTableRaw(db);

        Camera camera{0, "Entrance Cam", "Main entrance", "online"};
        int id = insertCameraRaw(db, camera);
        std::cout << "Inserted camera ID: " << id << '\n';

        auto found = findCameraByIdRaw(db, id);
        if (found)
        {
            printCamera(*found);
        }
        else
        {
            std::cout << "Camera not found\n";
        }
    }
    catch (...)
    {
        sqlite3_close(db);
        throw;
    }
    sqlite3_close(db);

    // ---------- 2. sqlite_orm ----------
    std::cout << "\n--- sqlite_orm ---\n";
    std::filesystem::remove(kOrmDb);

    auto storage = createStorage(kOrmDb);
    configureStorage(storage);

    Camera camera{0, "Entrance Cam", "Main entrance", "online"};
    int id = storage.insert(camera);
    std::cout << "Inserted camera ID: " << id << '\n';

    auto found = storage.get_optional<Camera>(id);
    if (found)
    {
        printCamera(*found);
    }
    else
    {
        std::cout << "Camera not found\n";
    }
}
