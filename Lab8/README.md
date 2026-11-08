# SurveillanceORM — лабораторна робота №8, варіант 12

Система відеоспостереження: **Camera** (1) — **RegisteredEvent** (N).
C++20, CMake, vcpkg (manifest), SQLite, sqlite_orm.

## Збирання та запуск (Windows, Visual Studio / Ninja)

1. Встановіть vcpkg і задайте змінну середовища `VCPKG_ROOT`.
2. (Один раз) зафіксуйте версію реєстру: `vcpkg x-update-baseline --add-initial-baseline`
   (додасть `builtin-baseline` у `vcpkg.json`).
3. Конфігурація і збирання:

       cmake --preset default
       cmake --build build

4. Запуск: `build\SurveillanceORM.exe`

Програма виконує послідовні демонстраційні сценарії (завдання 2–6).
Кожен сценарій створює власний файл `surveillance_*.db` і перед початком його
видаляє, тому повторний запуск не накопичує тестові записи.

## Структура

- `include/Camera.h`, `RegisteredEvent.h` — моделі даних
- `include/Database.h` — ORM-відображення (`make_storage/make_table/make_column`), `configureStorage()`
- `include/ICameraRepository.h`, `SQLiteCameraRepository.*` — Repository для Camera
- `include/EventStore.h`, `src/EventStore.cpp` — доступ до RegisteredEvent
- `src/RawSqliteCamera.cpp` — SQLite C API (лише порівняльна частина)
- `src/TestData.cpp` — тестовий набір (5 камер, 15 подій)
- `src/*Demo.cpp`, `src/main.cpp` — демонстраційні сценарії
