#include "Race.h"
#include <stdexcept>

Race::Race(std::string name, std::vector<const IParticipant*> results)
    : name_(std::move(name)), results_(std::move(results)) {}

std::size_t Race::position(const IParticipant* participant) const {
    for (std::size_t i = 0; i < results_.size(); ++i)
        if (results_[i] == participant) return i;
    throw std::invalid_argument("participant did not take part in the race");
}

int Race::getPoints(const IParticipant* participant, const IScoringSystem& scoring) const {
    return scoring.pointsForPosition(position(participant));
}

const IScoringSystem& Race::defaultScoring() {
    static const ClassicScoringSystem instance;
    return instance;
}
