#include "LeaderBoard.h"
#include <algorithm>

LeaderBoard::LeaderBoard(std::vector<Race*> races, const IScoringSystem& scoring)
    : races_(std::move(races)), scoring_(&scoring) {}

std::map<std::string, int> LeaderBoard::driverPoints() const {
    std::map<std::string, int> result;
    for (const Race* race : races_) {
        const auto& results = race->getResults();
        for (std::size_t i = 0; i < results.size(); ++i)
            result[results[i]->displayName()] += scoring_->pointsForPosition(i);
    }
    return result;
}

std::map<std::string, int> LeaderBoard::driverWins() const {
    std::map<std::string, int> wins;
    for (const Race* race : races_) {
        const auto& results = race->getResults();
        for (std::size_t i = 0; i < results.size(); ++i)
            wins[results[i]->displayName()] += (i == 0) ? 1 : 0;
    }
    return wins;
}

std::vector<std::string> LeaderBoard::driverRankings() const {
    const auto points = driverPoints();
    const auto wins = driverWins();
    std::vector<std::string> names;
    for (const auto& entry : points) names.push_back(entry.first);

    std::sort(names.begin(), names.end(), [&](const std::string& a, const std::string& b) {
        if (points.at(a) != points.at(b)) return points.at(a) > points.at(b);
        if (wins.at(a) != wins.at(b)) return wins.at(a) > wins.at(b);
        return a < b;
    });
    return names;
}
