#include "../include/LeaderBoard.h"

Leaderboard::Leaderboard(std::list<Race> const& races, std::shared_ptr<const IScoringSystem> scoring)
    : m_races(races), m_scoring(std::move(scoring))
{}

std::map<std::string, int> Leaderboard::driverResults() const
{
    std::map<std::string, int> results;
    for (Race const& race : m_races)
    {
        std::size_t index = 0;
        for (Race::ParticipantPtr const& participant : race.getResults())
        {
            results[participant->displayName()] += m_scoring->pointsForPosition(index++);
        }
    }
    return results;
}

std::map<std::string, int> Leaderboard::driverWins() const
{
    std::map<std::string, int> wins;
    for (Race const& race : m_races)
    {
        bool first = true;
        for (Race::ParticipantPtr const& participant : race.getResults())
        {
            wins[participant->displayName()] += first ? 1 : 0;
            first = false;
        }
    }
    return wins;
}

std::list<std::string> Leaderboard::driverRankings() const
{
    std::map<std::string, int> const points = driverResults();
    std::map<std::string, int> const wins = driverWins();

    std::list<std::string> names;
    for (auto const& entry : points)
    {
        names.push_back(entry.first);
    }
    names.sort([&](std::string const& a, std::string const& b)
    {
        if (points.at(a) != points.at(b)) return points.at(a) > points.at(b);
        if (wins.at(a) != wins.at(b)) return wins.at(a) > wins.at(b);
        return a < b;
    });
    return names;
}
