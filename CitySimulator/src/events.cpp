#include "events.hpp"
#include "services.hpp"

void EventService::onEnable()
{
}
void EventService::onDisable()
{
}

void EventService::registerListener(EventListener *listener, EventType eventType)
{
	Logger::logDebuggier(format("Registering listener for event %1%", std::to_string(eventType)));
	listeners[eventType].push_front(listener);
}
void EventService::unregisterListener(EventListener *listener, EventType eventType)
{
	listeners[eventType].remove(listener);
	Logger::logDebuggier(format("Unregistering listener for event %1%", std::to_string(eventType)));
}

void EventService::processQueue()
{
	// todo only process a subset according to a time limit/fixed maximum count

	size_t size = std::distance(pendingEvents.cbegin(), pendingEvents.cend());

	while (size--)
	{
		Event e = pendingEvents.front();
		pendingEvents.pop_front();

		auto eventListeners = listeners[e.type];
		for (EventListener *listener : eventListeners)
			listener->onEvent(e);

	}
}

void EventService::callEvent(const Event &event)
{
	pendingEvents.push_front(event);
}
