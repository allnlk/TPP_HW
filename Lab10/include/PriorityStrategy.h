#pragma once

#include <algorithm>
#include <memory>
#include <stdexcept>

#include "RegisteredEvent.h"

// Патерн Strategy: пріоритет події відеоспостереження.
// Параметри: важливість, тривалість, час очікування на опрацювання.

struct PriorityParams
{
    double importance;   // важливість (severity) 1..5
    double durationSec;  // тривалість події, с
    double waitingSec;   // час очікування на опрацювання, с
};

// Граничні значення для нормування параметрів до діапазону [0, 1]
inline constexpr double kMaxImportance = 5.0;
inline constexpr double kMaxDurationSec = 600.0;   // 10 хв
inline constexpr double kMaxWaitingSec = 3600.0;   // 1 год

inline double normalize(double value, double maxValue)
{
    return std::clamp(value / maxValue, 0.0, 1.0);
}

// Дані для стратегії формуються з сутності, отриманої через EventStore.
inline PriorityParams toPriorityParams(const RegisteredEvent& event, int waitingSec)
{
    return {static_cast<double>(event.severity),
            static_cast<double>(event.durationSec),
            static_cast<double>(waitingSec)};
}

// Strategy
class PriorityStrategy
{
public:
    virtual ~PriorityStrategy() = default;

    virtual double calculate(const PriorityParams& params) const = 0;
};

// 1. Максимум нормованих параметрів
class MaxNormalizedPriority : public PriorityStrategy
{
public:
    double calculate(const PriorityParams& params) const override
    {
        return std::max({normalize(params.importance, kMaxImportance),
                         normalize(params.durationSec, kMaxDurationSec),
                         normalize(params.waitingSec, kMaxWaitingSec)});
    }
};

// Вагові коефіцієнти для зважених алгоритмів
struct PriorityWeights
{
    double importance = 0.5;
    double duration = 0.2;
    double waiting = 0.3;
};

// 2. Зважена сума (параметри у вихідних одиницях)
class WeightedSumPriority : public PriorityStrategy
{
private:
    PriorityWeights weights;

public:
    explicit WeightedSumPriority(PriorityWeights value = {})
        : weights(value) {}

    double calculate(const PriorityParams& params) const override
    {
        return weights.importance * params.importance
             + weights.duration * params.durationSec
             + weights.waiting * params.waitingSec;
    }
};

// 3. Нормована зважена сума (результат у діапазоні [0, 1])
class NormalizedWeightedSumPriority : public PriorityStrategy
{
private:
    PriorityWeights weights;

public:
    explicit NormalizedWeightedSumPriority(PriorityWeights value = {})
        : weights(value)
    {
        if (weights.importance + weights.duration + weights.waiting <= 0.0)
        {
            throw std::invalid_argument("Sum of weights must be positive");
        }
    }

    double calculate(const PriorityParams& params) const override
    {
        double sum = weights.importance * normalize(params.importance, kMaxImportance)
                   + weights.duration * normalize(params.durationSec, kMaxDurationSec)
                   + weights.waiting * normalize(params.waitingSec, kMaxWaitingSec);
        return sum / (weights.importance + weights.duration + weights.waiting);
    }
};

// Context: використовує поточну стратегію, яку можна замінити під час роботи.
class EventPriorityCalculator
{
private:
    std::unique_ptr<PriorityStrategy> strategy;

public:
    explicit EventPriorityCalculator(std::unique_ptr<PriorityStrategy> initialStrategy)
        : strategy(std::move(initialStrategy)) {}

    void setStrategy(std::unique_ptr<PriorityStrategy> newStrategy)
    {
        strategy = std::move(newStrategy);
    }

    double calculate(const PriorityParams& params) const
    {
        return strategy->calculate(params);
    }
};
