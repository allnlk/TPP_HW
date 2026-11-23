#pragma once

#include <memory>
#include <stdexcept>
#include <string>

// Патерн State: стани зареєстрованої події відеоспостереження.
// Зареєстрована -> Перевірена -> Опрацьована; окремий стан - Відхилена.

// Виняток для недопустимого переходу між станами.
class InvalidTransitionError : public std::logic_error
{
public:
    using std::logic_error::logic_error;
};

class EventWorkflow;

// State: спільний інтерфейс операцій для всіх станів.
class EventState
{
public:
    virtual ~EventState() = default;

    virtual void verify(EventWorkflow& workflow) = 0;
    virtual void process(EventWorkflow& workflow) = 0;
    virtual void reject(EventWorkflow& workflow) = 0;

    virtual std::string name() const = 0;
};

// Context: делегує операції об'єкту поточного стану.
class EventWorkflow
{
private:
    std::unique_ptr<EventState> state;

public:
    explicit EventWorkflow(std::unique_ptr<EventState> initialState)
        : state(std::move(initialState)) {}

    void setState(std::unique_ptr<EventState> newState)
    {
        state = std::move(newState);
    }

    void verify()  { state->verify(*this); }
    void process() { state->process(*this); }
    void reject()  { state->reject(*this); }

    std::string stateName() const { return state->name(); }
};

// ConcreteState: Зареєстрована
class RegisteredState : public EventState
{
public:
    void verify(EventWorkflow& workflow) override;

    void process(EventWorkflow&) override
    {
        throw InvalidTransitionError("Event must be verified first");
    }

    void reject(EventWorkflow& workflow) override;

    std::string name() const override { return "Registered"; }
};

// ConcreteState: Перевірена
class VerifiedState : public EventState
{
public:
    void verify(EventWorkflow&) override
    {
        throw InvalidTransitionError("Event is already verified");
    }

    void process(EventWorkflow& workflow) override;
    void reject(EventWorkflow& workflow) override;

    std::string name() const override { return "Verified"; }
};

// ConcreteState: Опрацьована (кінцевий стан)
class ProcessedState : public EventState
{
public:
    void verify(EventWorkflow&) override
    {
        throw InvalidTransitionError("Event is already processed");
    }

    void process(EventWorkflow&) override
    {
        throw InvalidTransitionError("Event is already processed");
    }

    void reject(EventWorkflow&) override
    {
        throw InvalidTransitionError("Processed event cannot be rejected");
    }

    std::string name() const override { return "Processed"; }
};

// ConcreteState: Відхилена (окремий кінцевий стан)
class RejectedState : public EventState
{
public:
    void verify(EventWorkflow&) override
    {
        throw InvalidTransitionError("Event is rejected");
    }

    void process(EventWorkflow&) override
    {
        throw InvalidTransitionError("Event is rejected");
    }

    void reject(EventWorkflow&) override
    {
        throw InvalidTransitionError("Event is already rejected");
    }

    std::string name() const override { return "Rejected"; }
};

// Реалізація переходів між станами
inline void RegisteredState::verify(EventWorkflow& workflow)
{
    workflow.setState(std::make_unique<VerifiedState>());
}

inline void RegisteredState::reject(EventWorkflow& workflow)
{
    workflow.setState(std::make_unique<RejectedState>());
}

inline void VerifiedState::process(EventWorkflow& workflow)
{
    workflow.setState(std::make_unique<ProcessedState>());
}

inline void VerifiedState::reject(EventWorkflow& workflow)
{
    workflow.setState(std::make_unique<RejectedState>());
}
