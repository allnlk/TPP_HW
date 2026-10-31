#include "RootFindingTask.h"

#include <cmath>
#include <stdexcept>

void RootFindingTask::validate(double a, double b, double eps, int maxIter)
{
    if (!std::isfinite(a) || !std::isfinite(b) || a >= b)
        throw std::invalid_argument("Invalid interval: need finite a < b");

    if (!std::isfinite(b - a))
        throw std::overflow_error("Interval length overflow");

    if (!std::isfinite(eps) || eps <= 0.0)
        throw std::invalid_argument("eps must be finite and positive");

    if (maxIter <= 0)
        throw std::invalid_argument("maxIter must be positive");
}

RootFindingTask::RootFindingTask(double a, double b, double eps, int maxIter)
    : a_(a), b_(b), eps_(eps), maxIter_(maxIter)
{
    validate(a, b, eps, maxIter);
}

const RootResults& RootFindingTask::results() const
{
    if (!results_)
        throw std::logic_error("No results have been computed yet");
    return *results_;
}

void RootFindingTask::setInterval(double a, double b)
{
    // Підготовка: перевірка нових значень (стан ще не змінено)
    validate(a, b, eps_, maxIter_);

    // Фіксація: присвоєння double та reset() не кидають винятків
    a_ = a;
    b_ = b;
    results_.reset();
}

void RootFindingTask::setPrecision(double eps, int maxIter)
{
    validate(a_, b_, eps, maxIter);

    eps_ = eps;
    maxIter_ = maxIter;
    results_.reset();
}

void RootFindingTask::computeRoots(const Function& f, const Function& df,
                                   double x0)
{
    // Підготовка: усе обчислюється у локальних змінних.
    // Будь-який виняток тут залишає results_ без змін.
    const double bisectionRoot = bisection(f, a_, b_, eps_, maxIter_);
    const double newtonRoot = newton(f, df, x0, eps_, maxIter_);

    const RootResults computed{
        bisectionRoot,
        newtonRoot,
        residual(f, bisectionRoot),
        residual(f, newtonRoot)};

    // Фіксація: присвоєння optional<тривіальна структура> не кидає винятків
    results_ = computed;
}
