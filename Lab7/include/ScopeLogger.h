#pragma once

#include <iostream>
#include <string>
#include <utility>

// RAII-об'єкт для дослідження порядку створення і знищення локальних
// об'єктів під час розкручування стека (stack unwinding).
class ScopeLogger
{
private:
    std::string name_;

public:
    explicit ScopeLogger(std::string name)
        : name_(std::move(name))
    {
        std::cout << "[ENTER] " << name_ << '\n';
    }

    ~ScopeLogger() noexcept
    {
        std::cout << "[EXIT ] " << name_ << '\n';
    }

    ScopeLogger(const ScopeLogger&) = delete;
    ScopeLogger& operator=(const ScopeLogger&) = delete;
};
