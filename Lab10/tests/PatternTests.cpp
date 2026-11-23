#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>

#include "Database.h"
#include "EventObserver.h"
#include "EventProcessingService.h"
#include "EventState.h"
#include "EventStateFactory.h"
#include "EventStore.h"
#include "PriorityStrategy.h"
#include "SQLiteCameraRepository.h"
#include "StatusHistoryStore.h"

// ======================= State =======================

TEST(StateTest, MainPathRegisteredVerifiedProcessed)
{
    EventWorkflow workflow(std::make_unique<RegisteredState>());
    EXPECT_EQ(workflow.stateName(), "Registered");

    workflow.verify();
    EXPECT_EQ(workflow.stateName(), "Verified");

    workflow.process();
    EXPECT_EQ(workflow.stateName(), "Processed");
}

TEST(StateTest, RejectAllowedFromRegisteredAndVerified)
{
    EventWorkflow fromRegistered(std::make_unique<RegisteredState>());
    fromRegistered.reject();
    EXPECT_EQ(fromRegistered.stateName(), "Rejected");

    EventWorkflow fromVerified(std::make_unique<VerifiedState>());
    fromVerified.reject();
    EXPECT_EQ(fromVerified.stateName(), "Rejected");
}

TEST(StateTest, InvalidTransitionThrowsAndKeepsState)
{
    EventWorkflow workflow(std::make_unique<RegisteredState>());
    EXPECT_THROW(workflow.process(), InvalidTransitionError);
    EXPECT_EQ(workflow.stateName(), "Registered");

    workflow.verify();
    EXPECT_THROW(workflow.verify(), InvalidTransitionError);
    EXPECT_EQ(workflow.stateName(), "Verified");
}

TEST(StateTest, FinalStatesAllowNoTransitions)
{
    for (const char* status : {"Processed", "Rejected"})
    {
        EventWorkflow workflow(EventStateFactory::create(status));
        EXPECT_THROW(workflow.verify(), InvalidTransitionError);
        EXPECT_THROW(workflow.process(), InvalidTransitionError);
        EXPECT_THROW(workflow.reject(), InvalidTransitionError);
        EXPECT_EQ(workflow.stateName(), status);
    }
}

// =================== Factory Method ===================

TEST(FactoryTest, RestoresStateFromStoredStatus)
{
    for (const char* status : {"Registered", "Verified", "Processed", "Rejected"})
    {
        EXPECT_EQ(EventStateFactory::create(status)->name(), status);
    }
}

TEST(FactoryTest, UnknownStatusThrows)
{
    EXPECT_THROW(EventStateFactory::create("Unknown"), std::invalid_argument);
}

// ====================== Strategy ======================

TEST(StrategyTest, MaxNormalized)
{
    MaxNormalizedPriority strategy;
    // 2/5 = 0.4, 300/600 = 0.5, 1800/3600 = 0.5
    EXPECT_DOUBLE_EQ(strategy.calculate({2, 300, 1800}), 0.5);
    EXPECT_DOUBLE_EQ(strategy.calculate({5, 60, 0}), 1.0);
}

TEST(StrategyTest, WeightedSum)
{
    WeightedSumPriority strategy;  // ваги 0.5 / 0.2 / 0.3
    // 0.5*2 + 0.2*300 + 0.3*1800 = 1 + 60 + 540
    EXPECT_DOUBLE_EQ(strategy.calculate({2, 300, 1800}), 601.0);
}

TEST(StrategyTest, NormalizedWeightedSum)
{
    NormalizedWeightedSumPriority strategy;
    // 0.5*0.4 + 0.2*0.5 + 0.3*0.5 = 0.45
    EXPECT_DOUBLE_EQ(strategy.calculate({2, 300, 1800}), 0.45);
    // значення понад межу нормуються до 1
    EXPECT_DOUBLE_EQ(strategy.calculate({10, 10000, 100000}), 1.0);
}

TEST(StrategyTest, InvalidWeightsThrow)
{
    EXPECT_THROW(NormalizedWeightedSumPriority({0, 0, 0}), std::invalid_argument);
}

TEST(StrategyTest, StrategyCanBeReplacedAtRuntime)
{
    PriorityParams params{2, 300, 1800};
    EventPriorityCalculator calculator(std::make_unique<MaxNormalizedPriority>());
    EXPECT_DOUBLE_EQ(calculator.calculate(params), 0.5);

    calculator.setStrategy(std::make_unique<WeightedSumPriority>());
    EXPECT_DOUBLE_EQ(calculator.calculate(params), 601.0);

    calculator.setStrategy(std::make_unique<NormalizedWeightedSumPriority>());
    EXPECT_DOUBLE_EQ(calculator.calculate(params), 0.45);
}

TEST(StrategyTest, ParamsFromEntity)
{
    RegisteredEvent event{1, 1, "2026-11-01 10:00:00", "motion", 3, 90};
    PriorityParams params = toPriorityParams(event, 600);
    EXPECT_DOUBLE_EQ(params.importance, 3);
    EXPECT_DOUBLE_EQ(params.durationSec, 90);
    EXPECT_DOUBLE_EQ(params.waitingSec, 600);
}

// ====================== Observer ======================

namespace
{
SurveillanceEvent criticalEvent()
{
    return {SurveillanceEventType::CriticalEventConfirmed, 1, 1, "intrusion", 5};
}
}

TEST(ObserverTest, AllActiveSubscribersReceiveEvents)
{
    EventPublisher publisher;
    auto log = std::make_shared<CameraLog>();
    auto alerts = std::make_shared<AlertSystem>();
    publisher.subscribe(log);
    publisher.subscribe(alerts);

    publisher.notify({SurveillanceEventType::EventRegistered, 1, 1, "intrusion", 5});
    publisher.notify(criticalEvent());

    EXPECT_EQ(log->getEntries().size(), 2u);
    EXPECT_EQ(alerts->getAlertCount(), 1);  // реагує лише на критичну подію
}

TEST(ObserverTest, UnsubscribedObserverGetsNothing)
{
    EventPublisher publisher;
    auto log = std::make_shared<CameraLog>();
    auto alerts = std::make_shared<AlertSystem>();
    publisher.subscribe(log);
    publisher.subscribe(alerts);

    publisher.unsubscribe(alerts);
    publisher.notify(criticalEvent());

    EXPECT_EQ(alerts->getAlertCount(), 0);
    EXPECT_EQ(log->getEntries().size(), 1u);
}

TEST(ObserverTest, DestroyedObserverIsRemoved)
{
    EventPublisher publisher;
    auto log = std::make_shared<CameraLog>();
    publisher.subscribe(log);
    {
        auto alerts = std::make_shared<AlertSystem>();
        publisher.subscribe(alerts);
    }  // alerts знищено, підписка не продовжує його життя

    publisher.notify(criticalEvent());
    EXPECT_EQ(publisher.subscriberCount(), 1u);
    EXPECT_EQ(log->getEntries().size(), 1u);
}

// ========== Інтеграція: State + Repository + транзакції ==========

class ServiceTest : public ::testing::Test
{
protected:
    DbStorage storage = createStorage(":memory:");
    EventPublisher publisher;
    std::shared_ptr<CameraLog> log = std::make_shared<CameraLog>();
    std::shared_ptr<AlertSystem> alerts = std::make_shared<AlertSystem>();
    int cameraId = 0;

    void SetUp() override
    {
        configureStorage(storage);
        SQLiteCameraRepository cameras(storage);
        cameraId = cameras.add(Camera{0, "Test Cam", "Corridor", "online"});
        publisher.subscribe(log);
        publisher.subscribe(alerts);
    }

    static std::string fixedClock() { return "2026-11-23 10:00:00"; }

    RegisteredEvent makeEvent(int severity)
    {
        return {0, cameraId, "2026-11-23 09:00:00", "intrusion", severity, 120};
    }
};

TEST_F(ServiceTest, TransitionSavesStatusAndHistory)
{
    EventProcessingService service(storage, publisher, fixedClock);
    int id = service.registerEvent(makeEvent(2));

    EXPECT_EQ(service.verify(id), "Verified");
    EXPECT_EQ(service.process(id), "Processed");

    EventStore events(storage);
    EXPECT_EQ(events.findById(id)->status, "Processed");

    StatusHistoryStore history(storage);
    auto records = history.findByEvent(id);
    ASSERT_EQ(records.size(), 2u);
    EXPECT_EQ(records[0].oldStatus, "Registered");
    EXPECT_EQ(records[0].newStatus, "Verified");
    EXPECT_EQ(records[1].oldStatus, "Verified");
    EXPECT_EQ(records[1].newStatus, "Processed");
    EXPECT_EQ(records[1].changedAt, "2026-11-23 10:00:00");
}

TEST_F(ServiceTest, InvalidTransitionDoesNotChangeDatabase)
{
    EventProcessingService service(storage, publisher, fixedClock);
    int id = service.registerEvent(makeEvent(2));

    EXPECT_THROW(service.process(id), InvalidTransitionError);

    EventStore events(storage);
    StatusHistoryStore history(storage);
    EXPECT_EQ(events.findById(id)->status, "Registered");
    EXPECT_TRUE(history.findByEvent(id).empty());
}

TEST_F(ServiceTest, FailedHistoryWriteRollsBackStatus)
{
    // Помилка під час формування запису історії (друга операція транзакції)
    EventProcessingService service(storage, publisher,
        []() -> std::string { throw std::runtime_error("clock failure"); });
    int id = service.registerEvent(makeEvent(2));

    EXPECT_THROW(service.verify(id), std::runtime_error);

    EventStore events(storage);
    StatusHistoryStore history(storage);
    EXPECT_EQ(events.findById(id)->status, "Registered");  // ROLLBACK
    EXPECT_TRUE(history.findByEvent(id).empty());
}

TEST_F(ServiceTest, MissingEventThrows)
{
    EventProcessingService service(storage, publisher, fixedClock);
    EXPECT_THROW(service.verify(999), std::out_of_range);
}

TEST_F(ServiceTest, RegistrationNotifiesSubscribers)
{
    EventProcessingService service(storage, publisher, fixedClock);
    service.registerEvent(makeEvent(5));

    EXPECT_EQ(log->getEntries().size(), 1u);
    EXPECT_EQ(alerts->getAlertCount(), 0);
}

TEST_F(ServiceTest, VerifyingCriticalEventSendsAlert)
{
    EventProcessingService service(storage, publisher, fixedClock);
    int critical = service.registerEvent(makeEvent(5));
    int regular = service.registerEvent(makeEvent(2));

    service.verify(regular);
    EXPECT_EQ(alerts->getAlertCount(), 0);

    service.verify(critical);
    EXPECT_EQ(alerts->getAlertCount(), 1);
    EXPECT_EQ(log->getEntries().size(), 3u);  // 2 реєстрації + 1 критична
}

TEST_F(ServiceTest, NoAlertWhenTransitionFails)
{
    EventProcessingService service(storage, publisher, fixedClock);
    int id = service.registerEvent(makeEvent(5));
    service.reject(id);

    EXPECT_THROW(service.verify(id), InvalidTransitionError);
    EXPECT_EQ(alerts->getAlertCount(), 0);
}
