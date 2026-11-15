#pragma once

#include <list>
#include <map>
#include <memory>
#include <string>

#include "Driver.h"
#include "IScoringSystem.h"
#include "Race.h"
#include "SelfDrivingCar.h"

class Leaderboard
{
    std::list<Race> m_races;
    std::shared_ptr<const IScoringSystem> m_scoring;

public:
    // Сумісність: Leaderboard(races) працює як раніше (класична система балів).
    Leaderboard(std::list<Race> const& races,
                std::shared_ptr<const IScoringSystem> scoring = std::make_shared<ClassicScoringSystem>());

    std::map<std::string, int> driverResults() const;
    // Розширення: кількість перемог (1-х місць)
    std::map<std::string, int> driverWins() const;
    // Сортування: бали ↓, перемоги ↓ (додатковий критерій), ім'я ↑
    std::list<std::string> driverRankings() const;
};
