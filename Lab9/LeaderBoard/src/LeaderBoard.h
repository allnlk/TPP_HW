#pragma once
#include <map>
#include <string>
#include <vector>
#include "Driver.h"
#include "IScoringSystem.h"
#include "Race.h"
#include "SelfDrivingCar.h"

class LeaderBoard {
public:
    // Сумісність: LeaderBoard(races) працює як раніше (класична система балів).
    explicit LeaderBoard(std::vector<Race*> races,
                         const IScoringSystem& scoring = Race::defaultScoring());

    std::map<std::string, int> driverPoints() const;
    // Розширення: кількість перемог (1-х місць) кожного учасника
    std::map<std::string, int> driverWins() const;
    // Сортування: бали ↓, перемоги ↓ (додатковий критерій), ім'я ↑ (детермінованість)
    std::vector<std::string> driverRankings() const;

private:
    std::vector<Race*> races_;
    const IScoringSystem* scoring_;
};
