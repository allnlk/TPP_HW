add_library(library_core Library.cpp)
target_include_directories(library_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(library_app main.cpp)
target_link_libraries(library_app PRIVATE library_core)

add_executable(library_tests tests/LibraryTests.cpp)
target_link_libraries(library_tests PRIVATE library_core GTest::gtest_main)
add_test(NAME library_tests COMMAND library_tests)
