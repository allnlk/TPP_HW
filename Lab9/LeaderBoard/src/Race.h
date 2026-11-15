#pragma once
#include <cstddef>
#include <string>
#include <vector>
#include "IParticipant.h"
#include "IScoringSystem.h"

// Race зберігає лише результат заїзду. Імена та бали більше не обчислює (SRP).
// Вказівники нестрогі (не володіють об'єктами) — як і в початковому API.
class Race {
public:
    Race(std::string name, std::vector<const IParticipant*> results);

    const std::string& getName() const { return name_; }
    const std::vector<const IParticipant*>& getResults() const { return results_; }

    // 0-based позиція; std::invalid_argument, якщо учасника немає в заїзді
    std::size_t position(const IParticipant* participant) const;
    // Для сумісності з початковим API; за замовчуванням — класична система
    int getPoints(const IParticipant* participant,
                  const IScoringSystem& scoring = defaultScoring()) const;

    static const IScoringSystem& defaultScoring();

private:
    std::string name_;
    std::vector<const IParticipant*> results_;
};
