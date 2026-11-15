// Один і той самий файл компілюється двічі: проти legacy/ ("до") і проти src/ ("після").
// Використовує лише спільний публічний API, тому доводить збереження поведінки.
#include <gtest/gtest.h>
#include "LeaderBoard.h"

namespace {
struct Fixture {
    Driver driver1{"Nico Rosberg", "DE"};
    Driver driver2{"Lewis Hamilton", "UK"};
    Driver driver3{"Sebastian Vettel", "DE"};
    SelfDrivingCar car{"1.2", "ACME"};
};
}  // namespace

TEST(Characterization, RacePointsFollow25_18_15) {
    Fixture f;
    Race race("Australian GP", {&f.driver1, &f.driver2, &f.driver3});
    EXPECT_EQ(race.getPoints(&f.driver1), 25);
    EXPECT_EQ(race.getPoints(&f.driver2), 18);
    EXPECT_EQ(race.getPoints(&f.driver3), 15);
}

TEST(Characterization, EmptyLeaderBoard) {
    LeaderBoard board({});
    EXPECT_TRUE(board.driverPoints().empty());
    EXPECT_TRUE(board.driverRankings().empty());
}

TEST(Characterization, PointsAccumulateOverThreeRaces) {
    Fixture f;
    Race r1("Australian GP", {&f.driver1, &f.driver2, &f.driver3});
    Race r2("Malaysian GP", {&f.driver3, &f.driver2, &f.driver1});
    Race r3("Chinese GP", {&f.driver2, &f.driver1, &f.driver3});
    LeaderBoard board({&r1, &r2, &r3});

    auto points = board.driverPoints();
    EXPECT_EQ(points["Nico Rosberg"], 58);
    EXPECT_EQ(points["Lewis Hamilton"], 61);
    EXPECT_EQ(points["Sebastian Vettel"], 55);
    EXPECT_EQ(board.driverRankings(),
              (std::vector<std::string>{"Lewis Hamilton", "Nico Rosberg", "Sebastian Vettel"}));
}

TEST(Characterization, SelfDrivingCarHasSpecialName) {
    Fixture f;
    Race r("Monaco GP", {&f.car, &f.driver1, &f.driver2});
    LeaderBoard board({&r});
    auto points = board.driverPoints();
    EXPECT_EQ(points["Self Driving Car - ACME (1.2)"], 25);
    EXPECT_EQ(points["Nico Rosberg"], 18);
    EXPECT_EQ(points["Lewis Hamilton"], 15);
}

TEST(Characterization, RankingWithSelfDrivingCar) {
    Fixture f;
    Race r4("Monaco GP", {&f.car, &f.driver1, &f.driver2});
    Race r5("Spa GP", {&f.driver2, &f.car, &f.driver1});
    LeaderBoard board({&r4, &r5});
    EXPECT_EQ(board.driverRankings(),
              (std::vector<std::string>{"Self Driving Car - ACME (1.2)", "Lewis Hamilton",
                                        "Nico Rosberg"}));
}
