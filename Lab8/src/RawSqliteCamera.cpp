#include "RawSqliteCamera.h"

#include <stdexcept>
#include <string>

namespace
{
std::string columnText(sqlite3_stmt* stmt, int index)
{
    const unsigned char* text = sqlite3_column_text(stmt, index);
    return text ? std::string(reinterpret_cast<const char*>(text))
                : std::string();
}
}

void createCameraTableRaw(sqlite3* db)
{
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS Camera (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            location TEXT NOT NULL,
            status TEXT NOT NULL
        );
    )";

    char* errorMessage = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errorMessage);
    if (rc != SQLITE_OK)
    {
        std::string error = errorMessage ? errorMessage : "Unknown error";
        sqlite3_free(errorMessage);
        throw std::runtime_error(error);
    }
}

int insertCameraRaw(sqlite3* db, const Camera& camera)
{
    const char* sql = R"(
        INSERT INTO Camera (name, location, status)
        VALUES (?, ?, ?);
    )";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    rc = sqlite3_bind_text(stmt, 1, camera.name.c_str(), -1, SQLITE_TRANSIENT);
    if (rc == SQLITE_OK)
    {
        rc = sqlite3_bind_text(stmt, 2, camera.location.c_str(), -1, SQLITE_TRANSIENT);
    }
    if (rc == SQLITE_OK)
    {
        rc = sqlite3_bind_text(stmt, 3, camera.status.c_str(), -1, SQLITE_TRANSIENT);
    }
    if (rc != SQLITE_OK)
    {
        std::string error = sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        throw std::runtime_error(error);
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        std::string error = sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        throw std::runtime_error(error);
    }

    int id = static_cast<int>(sqlite3_last_insert_rowid(db));
    sqlite3_finalize(stmt);
    return id;
}

std::optional<Camera> findCameraByIdRaw(sqlite3* db, int id)
{
    const char* sql = R"(
        SELECT id, name, location, status
        FROM Camera
        WHERE id = ?;
    )";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    rc = sqlite3_bind_int(stmt, 1, id);
    if (rc != SQLITE_OK)
    {
        std::string error = sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        throw std::runtime_error(error);
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        Camera camera;
        camera.id = sqlite3_column_int(stmt, 0);
        camera.name = columnText(stmt, 1);
        camera.location = columnText(stmt, 2);
        camera.status = columnText(stmt, 3);
        sqlite3_finalize(stmt);
        return camera;
    }

    if (rc != SQLITE_DONE)
    {
        std::string error = sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        throw std::runtime_error(error);
    }

    sqlite3_finalize(stmt);
    return std::nullopt;
}
