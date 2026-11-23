#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Патерн Observer.
// Події: реєстрація події, підтвердження критичної події.
// Підписники: журнал камери, система попереджень.

enum class SurveillanceEventType
{
    EventRegistered,
    CriticalEventConfirmed
};

struct SurveillanceEvent
{
    SurveillanceEventType type;
    int eventId;
    int cameraId;
    std::string eventType;
    int severity;
};

// Observer
class Observer
{
public:
    virtual ~Observer() = default;

    virtual void update(const SurveillanceEvent& event) = 0;
};

// Subject: зберігає підписників і розсилає сповіщення.
// Логіки збереження даних не містить.
class EventPublisher
{
private:
    std::vector<std::weak_ptr<Observer>> observers;

public:
    void subscribe(const std::shared_ptr<Observer>& observer)
    {
        observers.push_back(observer);
    }

    void unsubscribe(const std::shared_ptr<Observer>& observer)
    {
        std::erase_if(observers,
            [&](const std::weak_ptr<Observer>& item) {
                auto ptr = item.lock();
                return !ptr || ptr == observer;
            });
    }

    void notify(const SurveillanceEvent& event)
    {
        std::erase_if(observers,
            [](const std::weak_ptr<Observer>& item) {
                return item.expired();
            });

        for (const auto& item : observers)
        {
            if (auto observer = item.lock())
            {
                observer->update(event);
            }
        }
    }

    std::size_t subscriberCount() const { return observers.size(); }
};

// ConcreteObserver: журнал камери - фіксує всі події
class CameraLog : public Observer
{
public:
    void update(const SurveillanceEvent& event) override
    {
        std::string text = "Camera " + std::to_string(event.cameraId)
            + ": event " + std::to_string(event.eventId)
            + " (" + event.eventType + ")"
            + (event.type == SurveillanceEventType::EventRegistered
                   ? " registered"
                   : " confirmed as critical");
        entries.push_back(text);
        std::cout << "[Camera log] " << text << '\n';
    }

    const std::vector<std::string>& getEntries() const { return entries; }

private:
    std::vector<std::string> entries;
};

// ConcreteObserver: система попереджень - реагує лише на критичні події
class AlertSystem : public Observer
{
public:
    void update(const SurveillanceEvent& event) override
    {
        if (event.type != SurveillanceEventType::CriticalEventConfirmed)
        {
            return;
        }
        ++alertCount;
        std::cout << "[ALERT] Critical event " << event.eventId
                  << " (" << event.eventType << ", severity "
                  << event.severity << ") on camera "
                  << event.cameraId << '\n';
    }

    int getAlertCount() const { return alertCount; }

private:
    int alertCount = 0;
};
