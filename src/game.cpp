#include <SFML/Window.hpp>
#include <numeric>
#include "game.hpp"
#include "logger.hpp"
#include "gamestate.hpp"
#include "config.hpp"
#include "input.hpp"

void FPSCounter::tick(float delta, sf::RenderWindow &window)
{
	backlog.push_back(delta);

	if (ticker.tick(delta))
	{
		int fps(0);

		double total = accumulate(backlog.begin(), backlog.end(), 0.0);
		if (total != 0)
		{
			double average = total / backlog.size();
			if (average != 0)
				fps = static_cast<int>(1.0 / average);
		}

		fpsText.setString(std::to_string(fps) + " FPS");

		backlog.clear();
	}

	window.draw(fpsText);
}


BaseGame::BaseGame(sf::RenderWindow &renderWindow) : window(renderWindow)
{
	limitFrameRate(true);

	// set icon
	setWindowIcon("icon.png");

	// load font
	if (!Constants::mainFont.loadFromFile("res/misc/font.ttf"))
	{
		Logger::logError("Font could not be loaded");
		exit(-1);
	}

	// key bindings
	window.setKeyRepeatEnabled(false);
	Globals::input = new Input;
	Globals::input->registerBindings();

	// set as global
	Globals::game = this;

	Logger::logDebug("Game started");
}

void BaseGame::beginGame()
{
	// initially fill screen
	window.clear(backgroundColour);
	window.display();

	start();

	fps.init(0.25);

	sf::Clock clock;
	sf::Event e;

	// todo separate physics from rendering

	while (window.isOpen())
	{
		Globals::input->advance();

		while (window.pollEvent(e))
		{
			if (e.type == sf::Event::Closed)
				window.close();

			// keys
			else if (e.type == sf::Event::KeyPressed || e.type == sf::Event::KeyReleased)
			{
				bool pressed = e.type == sf::Event::KeyPressed;

				if (pressed)
				{
					// escape to quit
					if (e.key.code == sf::Keyboard::Escape)
						window.close();
				}
				Globals::input->update(e.key.code, pressed);
				handleInput(e);
			}

			// everything else
			else
				handleInput(e);
		}

		float delta(clock.restart().asSeconds());
		tick(delta);

		window.clear(backgroundColour);
		render();

		// overlay
		if (showFPS)
		{
			sf::View view(window.getView());

			window.setView(window.getDefaultView());
			fps.tick(delta, window);
			window.setView(view);
		}

		window.display();
	}

	end();
}

void BaseGame::limitFrameRate(bool limit)
{
	window.setFramerateLimit(limit ? 60 : 0);
	window.setVerticalSyncEnabled(limit);
}

void BaseGame::setWindowIcon(const std::string &fileName)
{
	sf::Image icon;
	if (!icon.loadFromFile(Utils::searchForFile(fileName)))
	{
		Logger::logWarning("Could not load icon");
		return;
	}

	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
}


Game::Game(sf::RenderWindow &window) : BaseGame(window), current(nullptr)
{
	window.setTitle("Dank Game Memes");
	showFPS = true;
	limitFrameRate(Config::getBool("debug.limit-fps"));
}

Game::~Game()
{
	delete current;
}

void Game::start()
{
	switchState(StateType::GAME);
	box2DWorld = dynamic_cast<GameState*>(current)->getBox2DWorld();
}

void Game::tick(float delta)
{
	current->tick(delta);
}

void Game::render()
{
	current->render(window);

	if (box2DWorld != nullptr)
		box2DWorld->DrawDebugData();
}

void Game::handleInput(sf::Event e)
{
	current->handleInput(e);
}

void Game::switchState(StateType newStateType)
{
	// destruct current
	if (newStateType == StateType::NONE)
	{
		delete current;
		states.pop();
		current = states.top();
	}

	// push new state onto stack
	else
	{
		State *newState(createFromStateType(newStateType));
		if (!newState) return;

		bool shouldDestruct = (current && current->type != GAME && newStateType != PAUSE);

		if (shouldDestruct)
		{
			states.pop();
			delete current;
		}

		current = newState;
		states.push(newState);
	}

	window.setMouseCursorVisible(current->showMouse);
}

State* Game::createFromStateType(StateType type)
{
	switch (type)
	{
	case GAME: return new GameState;
	default:
		throw std::runtime_error("Not implemented");
	}
}

void Game::end()
{
	window.close();
}


bool ensureCWD(int argc, char **argv)
{
	using namespace boost::filesystem;
	const std::string required("res");

	if (!exists(current_path() / required))
	{
		// no args given
		if (argc != 2)
		{
			std::cerr << "Root directory not found. \nUsage: " << argv[0] << " <relative path to root dir>\nPress enter to quit." << std::endl;
			std::cin.get();
			return false;
		}

		// try supplied relative path
		std::string relativePath = argv[1];
		path newPath = current_path() / relativePath;

		// doesn't exist
		if (!exists(newPath))
		{
			std::cerr << "Invalid path" << std::endl;
			return false;
		}

		// update path and try again
		current_path(newPath);
		return ensureCWD(-1, nullptr);
	}

	return true;
}


void loadConfig(int &windowStyle)
{
	Config::loadConfig();

	int width, height;

	// borderless fullscreen
	if (Config::getBool("display.borderless-fullscreen"))
	{
		windowStyle = sf::Style::None;

		auto screenSize(sf::VideoMode::getDesktopMode());
		width = screenSize.width;
		height = screenSize.height;
	}

	// standard window
	else
	{
		windowStyle = sf::Style::Default;
		width = Config::getInt("display.resolution.width");
		height = Config::getInt("display.resolution.height");
	}

	Constants::setWindowSize(width, height);
}


int main(int argc, char **argv)
{
	try
	{
		// ensure that the program root is in the project root
		if (!ensureCWD(argc, argv))
			return -1;

		// create logger
		createLogger(std::cout, Logger::DEBUG);

		// load window size/style
		int style;
		loadConfig(style);

		sf::RenderWindow window(sf::VideoMode(Constants::windowSize.x, Constants::windowSize.y), "Game", style);

		BaseGame *game;
		game = new Game(window);
		dynamic_cast<BaseGame*>(game)->beginGame();

		Logger::logDebug("Shutdown cleanly");

		delete game;
	}
	catch (std::exception &e)
	{
		Logger::logError(std::string("An error occurred: ") + e.what());
	}
	catch (...)
	{
		Logger::logError("An unknown error occured");
	}
}