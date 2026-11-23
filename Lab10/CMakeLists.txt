cmake_minimum_required(VERSION 3.21)
project(SurveillancePatterns LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SqliteOrm CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

enable_testing()

# Ядро: моделі, ORM, Repository (з Lab8) + патерни (Lab10)
add_library(surveillance_core
    src/SQLiteCameraRepository.cpp
    src/EventStore.cpp
    src/TestData.cpp
    src/EventProcessingService.cpp
)
target_include_directories(surveillance_core
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(surveillance_core
    PUBLIC
        sqlite_orm::sqlite_orm
        unofficial::sqlite3::sqlite3
)

add_executable(SurveillancePatterns src/main.cpp)
target_link_libraries(SurveillancePatterns PRIVATE surveillance_core)

add_executable(pattern_tests tests/PatternTests.cpp)
target_link_libraries(pattern_tests PRIVATE surveillance_core GTest::gtest_main)
add_test(NAME pattern_tests COMMAND pattern_tests)

if(MSVC)
    target_compile_options(surveillance_core PRIVATE /utf-8 /W4)
    target_compile_options(SurveillancePatterns PRIVATE /utf-8 /W4)
    target_compile_options(pattern_tests PRIVATE /utf-8)
endif()
