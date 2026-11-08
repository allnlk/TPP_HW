#pragma once

#include <string>

#include <sqlite_orm/sqlite_orm.h>

#include "Camera.h"
#include "RegisteredEvent.h"

// ORM-відображення: структури C++ -> таблиці SQLite.
inline auto createStorage(const std::string& filename)
{
    using namespace sqlite_orm;

    return make_storage(
        filename,

        make_table(
            "Camera",
            make_column("id", &Camera::id, primary_key().autoincrement()),
            make_column("name", &Camera::name, not_null()),
            make_column("location", &Camera::location, not_null()),
            make_column("status", &Camera::status, not_null())
        ),

        make_table(
            "RegisteredEvent",
            make_column("id", &RegisteredEvent::id, primary_key().autoincrement()),
            make_column("cameraId", &RegisteredEvent::cameraId, not_null()),
            make_column("eventTime", &RegisteredEvent::eventTime, not_null()),
            make_column("eventType", &RegisteredEvent::eventType, not_null()),
            make_column("severity", &RegisteredEvent::severity, not_null()),
            make_column("durationSec", &RegisteredEvent::durationSec, not_null()),
            foreign_key(&RegisteredEvent::cameraId).references(&Camera::id)
        )
    );
}

using DbStorage = decltype(createStorage(""));

// Відкриває з'єднання, вмикає перевірку зовнішніх ключів
// та синхронізує схему бази даних.
inline void configureStorage(DbStorage& storage)
{
    storage.open_forever();
    storage.pragma.foreign_keys(true);
    storage.sync_schema();
}
