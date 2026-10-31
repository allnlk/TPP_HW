cmake_minimum_required(VERSION 3.21)
project(RootFindingLab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(RootFindingLib STATIC
    src/RootFinding.cpp
    src/RootFindingTask.cpp)
target_include_directories(RootFindingLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(RootDemo demo/main.cpp)
target_link_libraries(RootDemo PRIVATE RootFindingLib)

add_executable(AntipatternDemo antipattern/main.cpp)

find_package(GTest CONFIG REQUIRED)
enable_testing()
add_executable(RootTests tests/RootTests.cpp)
target_link_libraries(RootTests PRIVATE RootFindingLib GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(RootTests)
