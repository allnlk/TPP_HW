#pragma once

#include <stdexcept>
#include <string>

// Базовий клас усіх винятків бібліотеки
class RootFindingException : public std::runtime_error
{
public:
    explicit RootFindingException(const std::string& message)
        : std::runtime_error(message)
    {
    }
};

// Бісекція: на кінцях відрізка однаковий знак функції
class NoSignChangeException : public RootFindingException
{
private:
    double a_, b_, fa_, fb_;

public:
    NoSignChangeException(double a, double b, double fa, double fb)
        : RootFindingException("No sign change on interval"),
          a_(a), b_(b), fa_(fa), fb_(fb)
    {
    }

    double a() const noexcept { return a_; }
    double b() const noexcept { return b_; }
    double fa() const noexcept { return fa_; }
    double fb() const noexcept { return fb_; }
};

// Ньютон / січні: нульова похідна або нульовий знаменник
class ZeroDerivativeException : public RootFindingException
{
private:
    std::string method_;
    double point_;
    double value_;

public:
    ZeroDerivativeException(const std::string& method,
                            double point, double value)
        : RootFindingException("Zero derivative or denominator in " + method),
          method_(method), point_(point), value_(value)
    {
    }

    const std::string& method() const noexcept { return method_; }
    double point() const noexcept { return point_; }
    double value() const noexcept { return value_; }
};

// Точність не досягнута за maxIter ітерацій
class ConvergenceException : public RootFindingException
{
private:
    int maxIter_;
    double lastApproximation_;

public:
    ConvergenceException(int maxIter, double lastApproximation)
        : RootFindingException("Method did not converge"),
          maxIter_(maxIter), lastApproximation_(lastApproximation)
    {
    }

    int maxIter() const noexcept { return maxIter_; }
    double lastApproximation() const noexcept { return lastApproximation_; }
};
