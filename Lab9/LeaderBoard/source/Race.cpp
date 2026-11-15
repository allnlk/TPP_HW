#include "../include/Race.h"

#include <stdexcept>

Race::Race(std::string const& name, std::list<ParticipantPtr> const& participants)
    : m_name(name), m_results(participants)
{}

Race::Race(std::string const& name, std::list<Driver> const& drivers) : m_name(name)
{
    for (Driver const& driver : drivers)
    {
        m_results.push_back(std::make_shared<Driver>(driver));
    }
}

std::string Race::getName() const { return m_name; }

std::string Race::toString() const { return m_name; }

std::list<Race::ParticipantPtr> const& Race::getResults() const { return m_results; }

int Race::position(IParticipant const& participant) const
{
    int index = 0;
    for (ParticipantPtr const& p : m_results)
    {
        if (p->displayName() == participant.displayName())
        {
            return index;
        }
        ++index;
    }
    throw std::invalid_argument("participant did not take part in the race");
}

int Race::getPoints(IParticipant const& participant, IScoringSystem const& scoring) const
{
    return scoring.pointsForPosition(static_cast<std::size_t>(position(participant)));
}

IScoringSystem const& Race::defaultScoring()
{
    static ClassicScoringSystem const instance;
    return instance;
}
