cmake_minimum_required(VERSION 3.21)
project(SurveillanceORM LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SqliteOrm CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)

add_executable(SurveillanceORM
    src/main.cpp
    src/SQLiteCameraRepository.cpp
    src/EventStore.cpp
    src/TestData.cpp
    src/RawSqliteCamera.cpp
    src/ComparisonDemo.cpp
    src/SchemaDemo.cpp
    src/RepositoryDemo.cpp
    src/QueryDemo.cpp
    src/TransactionDemo.cpp
)

target_include_directories(SurveillanceORM
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(SurveillanceORM
    PRIVATE
        sqlite_orm::sqlite_orm
        unofficial::sqlite3::sqlite3
)

if(MSVC)
    target_compile_options(SurveillanceORM PRIVATE /utf-8 /W4)
endif()
