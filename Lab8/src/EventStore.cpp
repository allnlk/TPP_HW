#include "EventStore.h"

EventStore::EventStore(DbStorage& storage)
    : storage_(storage)
{
}

int EventStore::add(const RegisteredEvent& event)
{
    return storage_.insert(event);
}

std::optional<RegisteredEvent> EventStore::findById(int id)
{
    return storage_.get_optional<RegisteredEvent>(id);
}

std::vector<RegisteredEvent> EventStore::findAll()
{
    return storage_.get_all<RegisteredEvent>();
}

std::vector<RegisteredEvent> EventStore::findByCamera(int cameraId)
{
    using namespace sqlite_orm;

    return storage_.get_all<RegisteredEvent>(
        where(c(&RegisteredEvent::cameraId) == cameraId),
        order_by(&RegisteredEvent::eventTime)
    );
}

void EventStore::removeByCamera(int cameraId)
{
    using namespace sqlite_orm;

    storage_.remove_all<RegisteredEvent>(
        where(c(&RegisteredEvent::cameraId) == cameraId)
    );
}
