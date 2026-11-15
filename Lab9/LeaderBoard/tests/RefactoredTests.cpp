// Модульні тести нової архітектури: LeaderBoard тестується ізольовано, без Driver/SelfDrivingCar.
#include <gtest/gtest.h>
#include "LeaderBoard.h"

namespace {
class FakeParticipant : public IParticipant {
public:
    explicit FakeParticipant(std::string n) : name(std::move(n)) {}
    std::string displayName() const override { return name; }
    std::string name;
};
}  // namespace

TEST(LeaderBoardIsolated, WorksWithAnyIParticipant) {
    FakeParticipant a("A"), b("B");
    Race race("R1", {&a, &b});
    LeaderBoard board({&race});
    EXPECT_EQ(board.driverPoints(), (std::map<std::string, int>{{"A", 25}, {"B", 18}}));
}

TEST(LeaderBoardIsolated, ParticipantBeyondThirdPlaceGetsZeroInClassicSystem) {
    FakeParticipant a("A"), b("B"), c("C"), d("D");
    Race race("R1", {&a, &b, &c, &d});
    LeaderBoard board({&race});
    EXPECT_EQ(board.driverPoints().at("D"), 0);
}

TEST(LeaderBoardIsolated, ScoringSystemCanBeReplaced) {
    FakeParticipant a("A"), b("B"), c("C"), d("D");
    Race race("R1", {&a, &b, &c, &d});
    ExtendedScoringSystem extended;
    LeaderBoard board({&race}, extended);
    EXPECT_EQ(board.driverPoints().at("D"), 12);
}

TEST(LeaderBoardIsolated, CustomScoringSystemViaTable) {
    FakeParticipant a("A"), b("B");
    Race race("R1", {&a, &b});
    TableScoringSystem scoring({3, 1});
    LeaderBoard board({&race}, scoring);
    EXPECT_EQ(board.driverPoints(), (std::map<std::string, int>{{"A", 3}, {"B", 1}}));
}

TEST(ParticipantNames, DriverAndSelfDrivingCar) {
    Driver d("Lewis Hamilton", "UK");
    SelfDrivingCar car("1.2", "ACME");
    EXPECT_EQ(d.displayName(), "Lewis Hamilton");
    EXPECT_EQ(car.displayName(), "Self Driving Car - ACME (1.2)");
}

TEST(RaceTest, PositionOfUnknownParticipantThrows) {
    FakeParticipant a("A"), stranger("X");
    Race race("R1", {&a});
    EXPECT_THROW(race.position(&stranger), std::invalid_argument);
}

// ---- Індивідуальне розширення: додатковий критерій при рівності балів ----
TEST(TieBreak, MoreWinsRanksHigherWhenPointsAreEqual) {
    FakeParticipant a("A"), b("B");
    TableScoringSystem flat({10, 10});  // усі отримують однакові бали
    Race r1("R1", {&a, &b}), r2("R2", {&a, &b}), r3("R3", {&b, &a});
    LeaderBoard board({&r1, &r2, &r3}, flat);
    EXPECT_EQ(board.driverPoints().at("A"), board.driverPoints().at("B"));
    EXPECT_EQ(board.driverWins(), (std::map<std::string, int>{{"A", 2}, {"B", 1}}));
    EXPECT_EQ(board.driverRankings(), (std::vector<std::string>{"A", "B"}));
}

TEST(TieBreak, WinsBeatAlphabeticalOrder) {
    FakeParticipant a("A"), b("B");
    TableScoringSystem flat({10, 10});
    Race r1("R1", {&b, &a}), r2("R2", {&b, &a}), r3("R3", {&a, &b});
    LeaderBoard board({&r1, &r2, &r3}, flat);
    EXPECT_EQ(board.driverRankings(), (std::vector<std::string>{"B", "A"}));
}

TEST(TieBreak, FullTieFallsBackToNameForDeterministicOrder) {
    FakeParticipant a("A"), b("B");
    TableScoringSystem flat({10, 10});
    Race r1("R1", {&b, &a}), r2("R2", {&a, &b});
    LeaderBoard board({&r1, &r2}, flat);
    EXPECT_EQ(board.driverRankings(), (std::vector<std::string>{"A", "B"}));
}

TEST(TieBreak, PointsStillHavePriorityOverWins) {
    FakeParticipant a("A"), b("B");
    Race r1("R1", {&a, &b}), r2("R2", {&b, &a}), r3("R3", {&b, &a});
    // Класична система: A = 25+18+18 = 61, B = 18+25+25 = 68 → бали важливіші за решту критеріїв
    LeaderBoard board({&r1, &r2, &r3});
    EXPECT_EQ(board.driverRankings(), (std::vector<std::string>{"B", "A"}));
}
