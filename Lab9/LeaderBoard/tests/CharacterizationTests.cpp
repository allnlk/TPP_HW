// Один файл компілюється двічі: проти legacy/ ("до") і проти нового коду ("після").
// Використовує лише спільний публічний API, тому доводить збереження поведінки.
#include <gtest/gtest.h>
#include "LeaderBoard.h"

namespace {
struct Fixture {
    Driver driver1{"Nico Rosberg", "DE"};
    Driver driver2{"Lewis Hamilton", "UK"};
    Driver driver3{"Sebastian Vettel", "DE"};
};
using Names = std::list<std::string>;
}  // namespace

TEST(Characterization, RacePointsFollow25_18_15) {
    Fixture f;
    Race race("Australian GP", {f.driver1, f.driver2, f.driver3});
    EXPECT_EQ(race.getPoints(f.driver1), 25);
    EXPECT_EQ(race.getPoints(f.driver2), 18);
    EXPECT_EQ(race.getPoints(f.driver3), 15);
}

TEST(Characterization, EmptyLeaderboard) {
    Leaderboard board(std::list<Race>{});
    EXPECT_TRUE(board.driverResults().empty());
    EXPECT_TRUE(board.driverRankings().empty());
}

TEST(Characterization, SingleDriverGetsWinnerPoints) {
    Fixture f;
    Race race("Solo GP", {f.driver1});
    Leaderboard board(std::list<Race>{race});
    EXPECT_EQ(board.driverResults()["Nico Rosberg"], 25);
}

TEST(Characterization, PointsAccumulateOverThreeRaces) {
    Fixture f;
    Race r1("Australian GP", {f.driver1, f.driver2, f.driver3});
    Race r2("Malaysian GP", {f.driver3, f.driver2, f.driver1});
    Race r3("Chinese GP", {f.driver2, f.driver1, f.driver3});
    Leaderboard board(std::list<Race>{r1, r2, r3});

    auto points = board.driverResults();
    EXPECT_EQ(points["Nico Rosberg"], 58);
    EXPECT_EQ(points["Lewis Hamilton"], 61);
    EXPECT_EQ(points["Sebastian Vettel"], 55);
    EXPECT_EQ(board.driverRankings(),
              (Names{"Lewis Hamilton", "Nico Rosberg", "Sebastian Vettel"}));
}

TEST(Characterization, EqualPointsKeepAlphabeticalOrder) {
    Fixture f;
    Race r1("R1", {f.driver1, f.driver2});   // Rosberg 25, Hamilton 18
    Race r2("R2", {f.driver2, f.driver1});   // Hamilton 25, Rosberg 18 -> 43 : 43
    Leaderboard board(std::list<Race>{r1, r2});
    EXPECT_EQ(board.driverResults()["Nico Rosberg"], board.driverResults()["Lewis Hamilton"]);
    EXPECT_EQ(board.driverRankings(), (Names{"Lewis Hamilton", "Nico Rosberg"}));
}

TEST(Characterization, RaceName) {
    Fixture f;
    Race race("Monaco GP", {f.driver1});
    EXPECT_EQ(race.toString(), "Monaco GP");
}
