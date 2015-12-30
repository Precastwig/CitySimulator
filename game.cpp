#include <boost/filesystem.hpp>
#include <iostream>
#include "game.hpp"
#include "services.hpp"

bool ensureCWD(int argc, char **argv)
{
	const std::string required("res");

	if (!boost::filesystem::exists(boost::filesystem::current_path() / required))
	{
		// no args given
		if (argc != 2)
		{
			std::cerr << "Root directory not found. \nUsage: " << argv[0] << " <relative path to root dir>" << std::endl;
			return false;
		}

		// try supplied relative path
		std::string relativePath = argv[1];
		boost::filesystem::path newPath = boost::filesystem::current_path() / relativePath;

		// doesn't exist
		if (!boost::filesystem::exists(newPath))
		{
			std::cerr << "Invalid path" << std::endl;
			return false;
		}

		// update path and try again
		boost::filesystem::current_path(newPath);
		return ensureCWD(-1, nullptr);
	}

	return true;
}


void loadConfig(int &windowStyle)
{
	auto config = new ConfigService(Constants::referenceConfigPath, Constants::configPath);
	Locator::provide(SERVICE_CONFIG, config);

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

		// create game
		Game game(window);
		game.beginGame();
		game.endGame();

		Logger::logInfo("Shutdown cleanly");
		return 0;
	}
	catch (std::exception &e)
	{
		Logger::logError(std::string("An error occurred: ") + e.what());
		return 1;
	}
	catch (...)
	{
		Logger::logError("An unknown error occurred");
		return 2;
	}
}