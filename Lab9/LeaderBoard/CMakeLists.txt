# ---- Legacy (дослівно з Racing-Car-Katas), лише для тестів "до рефакторингу" ----
add_library(leaderboard_legacy
    legacy/source/Driver.cpp legacy/source/Race.cpp
    legacy/source/SelfDrivingCar.cpp legacy/source/LeaderBoard.cpp)
target_include_directories(leaderboard_legacy PUBLIC legacy/include)

add_executable(leaderboard_legacy_tests
    tests/CharacterizationTests.cpp tests/LegacyQuirkTests.cpp)
target_link_libraries(leaderboard_legacy_tests PRIVATE leaderboard_legacy GTest::gtest_main)
add_test(NAME leaderboard_legacy_tests COMMAND leaderboard_legacy_tests)

# ---- Рефакторена версія ----
add_library(leaderboard_core
    source/Driver.cpp source/Race.cpp
    source/SelfDrivingCar.cpp source/LeaderBoard.cpp)
target_include_directories(leaderboard_core PUBLIC include)

add_executable(leaderboard_characterization_tests tests/CharacterizationTests.cpp)
target_link_libraries(leaderboard_characterization_tests PRIVATE leaderboard_core GTest::gtest_main)
add_test(NAME leaderboard_characterization_tests COMMAND leaderboard_characterization_tests)

add_executable(leaderboard_tests tests/RefactoredTests.cpp)
target_link_libraries(leaderboard_tests PRIVATE leaderboard_core GTest::gtest_main)
add_test(NAME leaderboard_tests COMMAND leaderboard_tests)
