#ifndef CITYSIM_EVENTS_HPP
#define CITYSIM_EVENTS_HPP

#include <SFML/Window.hpp>

enum EventType
{
	INPUT_KEY,
	INPUT_CLICK,

	HUMAN_SPAWN,
	HUMAN_DEATH,
	HUMAN_INTERACT,

	EVENT_UNKNOWN
};

struct Event
{
	Event(EventType type);

	EventType type;
};

struct EventListener
{
	virtual void onEvent(const Event &event) = 0;
};

struct InputKeyEvent : public Event
{
	InputKeyEvent(sf::Keyboard::Key key, bool pressed);

	sf::Keyboard::Key key;
	bool pressed;
};

#endif