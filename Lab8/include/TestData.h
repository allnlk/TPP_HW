#pragma once

#include <string>

// Видаляє файл бази даних (якщо існує), створює його заново
// та заповнює тестовими даними: 5 камер і 15 подій.
void prepareTestData(const std::string& databaseFile);
