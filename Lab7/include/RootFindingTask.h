#pragma once

#include "RootFinding.h"

#include <optional>

// Збережені результати обчислення двома методами
struct RootResults
{
    double bisectionRoot;
    double newtonRoot;
    double bisectionResidual;
    double newtonResidual;
};

// Інваріанти класу (виконуються для кожного доступного об'єкта):
//   1. a_ і b_ скінченні, a_ < b_, довжина (b_ - a_) скінченна
//   2. eps_ скінченне і eps_ > 0
//   3. maxIter_ > 0
//   4. results_ або порожні, або відповідають поточним a_, b_
class RootFindingTask
{
private:
    double a_;
    double b_;
    double eps_;
    int maxIter_;
    std::optional<RootResults> results_;

    static void validate(double a, double b, double eps, int maxIter);

public:
    // Кидає std::invalid_argument або std::overflow_error;
    // у разі винятку об'єкт не створюється
    RootFindingTask(double a, double b, double eps, int maxIter);

    double a() const noexcept { return a_; }
    double b() const noexcept { return b_; }
    double eps() const noexcept { return eps_; }
    int maxIter() const noexcept { return maxIter_; }
    bool hasResults() const noexcept { return results_.has_value(); }

    // Кидає std::logic_error, якщо результатів ще немає
    const RootResults& results() const;

    // Гарантія: STRONG. Перевірка до зміни стану.
    // Після успішної зміни старі результати скидаються.
    void setInterval(double a, double b);

    // Гарантія: STRONG. Перевірка до зміни стану.
    // Після успішної зміни старі результати скидаються.
    void setPrecision(double eps, int maxIter);

    // Гарантія: STRONG. Обчислює корінь методами бісекції та Ньютона
    // у тимчасових змінних; результати записуються лише після
    // успішного завершення обох методів і обчислення нев'язок.
    void computeRoots(const Function& f, const Function& df, double x0);

    // Гарантія: NO-THROW
    void clearResults() noexcept { results_.reset(); }
};
