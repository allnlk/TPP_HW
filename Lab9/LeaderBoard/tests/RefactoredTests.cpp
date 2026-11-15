// Модульні тести нової архітектури: Leaderboard тестується ізольовано, без Driver/SelfDrivingCar.
#include <gtest/gtest.h>
#include "LeaderBoard.h"

namespace {
class FakeParticipant : public IParticipant {
public:
    explicit FakeParticipant(std::string n) : name(std::move(n)) {}
    std::string displayName() const override { return name; }
    std::string name;
};
Race::ParticipantPtr P(std::string name) { return std::make_shared<FakeParticipant>(std::move(name)); }
using Names = std::list<std::string>;
using Points = std::map<std::string, int>;
}  // namespace

TEST(LeaderboardIsolated, WorksWithAnyIParticipant) {
    Race race("R1", {P("A"), P("B")});
    Leaderboard board(std::list<Race>{race});
    EXPECT_EQ(board.driverResults(), (Points{{"A", 25}, {"B", 18}}));
}

TEST(LeaderboardIsolated, FourthPlaceGetsZeroInClassicSystem) {
    Race race("R1", {P("A"), P("B"), P("C"), P("D")});   // у legacy це вихід за межі масиву (UB)
    Leaderboard board(std::list<Race>{race});
    EXPECT_EQ(board.driverResults().at("D"), 0);
}

TEST(LeaderboardIsolated, ScoringSystemCanBeReplaced) {
    Race race("R1", {P("A"), P("B"), P("C"), P("D")});
    Leaderboard board(std::list<Race>{race}, std::make_shared<ExtendedScoringSystem>());
    EXPECT_EQ(board.driverResults().at("D"), 12);
}

TEST(LeaderboardIsolated, CustomScoringSystemViaTable) {
    Race race("R1", {P("A"), P("B")});
    Leaderboard board(std::list<Race>{race},
                      std::make_shared<TableScoringSystem>(std::vector<int>{3, 1}));
    EXPECT_EQ(board.driverResults(), (Points{{"A", 3}, {"B", 1}}));
}

TEST(SelfDrivingCarFix, KeepsSpecialNameBecauseNoSlicing) {
    Race::ParticipantPtr driver = std::make_shared<Driver>("Lewis Hamilton", "UK");
    Race::ParticipantPtr car = std::make_shared<SelfDrivingCar>("1.2", "ACME");
    Race race("Monaco GP", {car, driver});
    Leaderboard board(std::list<Race>{race});
    EXPECT_EQ(board.driverResults(),
              (Points{{"Self Driving Car - ACME (1.2)", 25}, {"Lewis Hamilton", 18}}));
}

TEST(SelfDrivingCarFix, RankingWithSelfDrivingCar) {
    Race::ParticipantPtr d1 = std::make_shared<Driver>("Nico Rosberg", "DE");
    Race::ParticipantPtr d2 = std::make_shared<Driver>("Lewis Hamilton", "UK");
    Race::ParticipantPtr car = std::make_shared<SelfDrivingCar>("1.2", "ACME");
    Race r4("Monaco GP", {car, d1, d2});
    Race r5("Spa GP", {d2, car, d1});
    Leaderboard board(std::list<Race>{r4, r5});
    EXPECT_EQ(board.driverRankings(),
              (Names{"Self Driving Car - ACME (1.2)", "Lewis Hamilton", "Nico Rosberg"}));
}

TEST(ParticipantNames, DriverAndSelfDrivingCar) {
    Driver d("Lewis Hamilton", "UK");
    SelfDrivingCar car("1.2", "ACME");
    EXPECT_EQ(d.displayName(), "Lewis Hamilton");
    EXPECT_EQ(car.displayName(), "Self Driving Car - ACME (1.2)");
    car.setAlgorithmVersion("2.0");
    EXPECT_EQ(car.displayName(), "Self Driving Car - ACME (2.0)");
}

TEST(RaceTest, PositionOfUnknownParticipantThrows) {
    Race race("R1", {P("A")});
    FakeParticipant stranger("X");
    EXPECT_THROW(race.position(stranger), std::invalid_argument);
}

// ---- Індивідуальне розширення: додатковий критерій при рівності балів ----
TEST(TieBreak, MoreWinsRanksHigherWhenPointsAreEqual) {
    auto a = P("A"), b = P("B");
    Race r1("R1", {a, b}), r2("R2", {a, b}), r3("R3", {b, a});
    Leaderboard board(std::list<Race>{r1, r2, r3},
                      std::make_shared<TableScoringSystem>(std::vector<int>{10, 10}));
    EXPECT_EQ(board.driverResults().at("A"), board.driverResults().at("B"));
    EXPECT_EQ(board.driverWins(), (Points{{"A", 2}, {"B", 1}}));
    EXPECT_EQ(board.driverRankings(), (Names{"A", "B"}));
}

TEST(TieBreak, WinsBeatAlphabeticalOrder) {
    auto a = P("A"), b = P("B");
    Race r1("R1", {b, a}), r2("R2", {b, a}), r3("R3", {a, b});
    Leaderboard board(std::list<Race>{r1, r2, r3},
                      std::make_shared<TableScoringSystem>(std::vector<int>{10, 10}));
    EXPECT_EQ(board.driverRankings(), (Names{"B", "A"}));
}

TEST(TieBreak, FullTieFallsBackToName) {
    auto a = P("A"), b = P("B");
    Race r1("R1", {b, a}), r2("R2", {a, b});
    Leaderboard board(std::list<Race>{r1, r2},
                      std::make_shared<TableScoringSystem>(std::vector<int>{10, 10}));
    EXPECT_EQ(board.driverRankings(), (Names{"A", "B"}));
}

TEST(TieBreak, PointsStillHavePriorityOverWins) {
    auto a = P("A"), b = P("B");
    Race r1("R1", {a, b}), r2("R2", {b, a}), r3("R3", {b, a});
    // Класична система: A = 25+18+18 = 61, B = 18+25+25 = 68
    Leaderboard board(std::list<Race>{r1, r2, r3});
    EXPECT_EQ(board.driverRankings(), (Names{"B", "A"}));
}
