#pragma once

#include <vector>

#include "Database.h"
#include "EventStatusHistory.h"

// Lab10: доступ до таблиці історії станів (за зразком EventStore).
class StatusHistoryStore
{
public:
    explicit StatusHistoryStore(DbStorage& storage)
        : storage_(storage) {}

    int add(const EventStatusHistory& record)
    {
        return storage_.insert(record);
    }

    std::vector<EventStatusHistory> findByEvent(int eventId)
    {
        using namespace sqlite_orm;
        return storage_.get_all<EventStatusHistory>(
            where(c(&EventStatusHistory::eventId) == eventId),
            order_by(&EventStatusHistory::id)
        );
    }

private:
    DbStorage& storage_;
};
