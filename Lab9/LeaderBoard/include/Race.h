#pragma once

#include <list>
#include <memory>
#include <string>

#include "Driver.h"
#include "IParticipant.h"
#include "IScoringSystem.h"

// Race зберігає лише результат заїзду. Бали та імена більше не обчислює (SRP).
// Учасники зберігаються поліморфно (shared_ptr), тому slicing неможливий.
class Race
{
public:
    using ParticipantPtr = std::shared_ptr<const IParticipant>;

    Race(std::string const& name, std::list<ParticipantPtr> const& participants);
    // Сумісність з початковим API: Race(name, std::list<Driver>)
    Race(std::string const& name, std::list<Driver> const& drivers);

    std::string getName() const;
    std::string toString() const;
    std::list<ParticipantPtr> const& getResults() const;

    // 0-based позиція; std::invalid_argument, якщо учасника немає в заїзді.
    // Учасники ідентифікуються за displayName() — так само, як і в рейтингу.
    int position(IParticipant const& participant) const;
    int getPoints(IParticipant const& participant,
                  IScoringSystem const& scoring = defaultScoring()) const;

    static IScoringSystem const& defaultScoring();

private:
    std::string m_name;
    std::list<ParticipantPtr> m_results;
};
