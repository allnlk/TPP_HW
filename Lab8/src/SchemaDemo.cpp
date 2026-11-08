#include <iostream>

#include "Database.h"
#include "Demos.h"
#include "Printing.h"
#include "TestData.h"

namespace
{
void printTableInfo(DbStorage& storage, const std::string& table)
{
    std::cout << "Table " << table << " exists: " << std::boolalpha
              << storage.table_exists(table) << '\n';

    for (const auto& column : storage.table_info(table))
    {
        std::cout << "  " << column.name << " " << column.type
                  << " notnull=" << static_cast<int>(column.notnull)
                  << " pk=" << column.pk << '\n';
    }
}
}

void runSchemaDemo()
{
    using namespace sqlite_orm;

    std::cout << "\n=== TASK 3: DATA MODEL AND ORM MAPPING ===\n\n";
    prepareTestData(kSchemaDb);

    auto storage = createStorage(kSchemaDb);
    configureStorage(storage);

    std::cout << "\nTables created by sync_schema():\n";
    printTableInfo(storage, "Camera");
    printTableInfo(storage, "RegisteredEvent");

    std::cout << "\nRecords linked to camera 1 (1:N relationship):\n";
    auto events = storage.get_all<RegisteredEvent>(
        where(c(&RegisteredEvent::cameraId) == 1)
    );
    for (const auto& event : events)
    {
        printEvent(event);
    }

    std::cout << "\nForeign key check (camera 9999 does not exist):\n";
    try
    {
        RegisteredEvent invalid{0, 9999, "2026-11-01 10:00:00", "motion", 1, 5};
        storage.insert(invalid);
        std::cout << "Unexpected: insertion succeeded\n";
    }
    catch (const std::exception& ex)
    {
        std::cout << "Insertion rejected: " << ex.what() << '\n';
    }
}
