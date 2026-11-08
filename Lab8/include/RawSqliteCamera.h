#pragma once

#include <optional>

#include <sqlite3.h>

#include "Camera.h"

// Єдине місце, де використовується SQLite C API (порівняльна частина роботи).
void createCameraTableRaw(sqlite3* db);
int insertCameraRaw(sqlite3* db, const Camera& camera);
std::optional<Camera> findCameraByIdRaw(sqlite3* db, int id);
