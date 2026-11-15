// ТІЛЬКИ для legacy: фіксує дефекти початкового коду, які рефакторинг усуває.
#include <gtest/gtest.h>
#include "LeaderBoard.h"
#include "SelfDrivingCar.h"

TEST(LegacyQuirk, SelfDrivingCarIsSlicedAndLosesItsSpecialName) {
    Driver driver("Lewis Hamilton", "UK");
    SelfDrivingCar car("1.2", "ACME");
    Race race("Monaco GP", {driver, car});   // list<Driver> за значенням -> slicing
    Leaderboard board(std::list<Race>{race});
    auto results = board.driverResults();
    EXPECT_EQ(results.count("1.2"), 1u);                                // ім'я = версія алгоритму
    EXPECT_EQ(results.count("Self Driving Car - ACME (1.2)"), 0u);      // очікуване ім'я ніколи не з'являється
}
