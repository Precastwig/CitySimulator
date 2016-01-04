#ifndef CITYSIM_STATE_HPP
#define CITYSIM_STATE_HPP

namespace sf
{
	class RenderWindow;
}

enum StateType
{
	NONE,
	MENU,
	OPTIONS,
	GAME,
	PAUSE,
	GAMEOVER
};

class State
{
public:
	virtual ~State()
	{
	}


	State(StateType screenType, bool mouse = true) : type(screenType), showMouse(mouse)
	{
	}

	virtual void tick(float delta) = 0;

	virtual void render(sf::RenderWindow &window) = 0;

	const StateType type;
	bool showMouse;
};

#endif