#ifndef CITYSIMULATOR_BUILDING_HPP
#define CITYSIMULATOR_BUILDING_HPP

#include <SFML/Graphics/Rect.hpp>
#include <map>

class World;

/**
 * A building entrance/exit, either inside or outside
 */
struct Door
{
	int id;
	World *ownedWorld;
	sf::Vector2i localTilePos;


	Door(int id, World *ownedWorld, const sf::Vector2i &localTilePos)
			: id(id), ownedWorld(ownedWorld), localTilePos(localTilePos)
	{
	}
};

class Building
{
public:
	Building(World &world, const sf::IntRect &tileBounds, int id, std::string buildingWorldName)
			: outsideWorld(&world), buildingID(id), insideWorldName(buildingWorldName), bounds(tileBounds)
	{
	}

	void discoverWindows();

	bool isWindowLit(const sf::Vector2i &tile);

	void setWindowLight(const sf::Vector2i &tile, bool lit);

	/**
	 * Adds a door to the building
	 * @param doorID The ID of this door and its partner
	 * @param doorTilePos The door's tile position in its own world
	 * @param doorWorld The door's own world, which must be either insideWorld or outsideWorld
	 */
	void addDoor(int doorID, const sf::Vector2i &doorTilePos, World *doorWorld);

	Door *getDoorByTile(const sf::Vector2i &tile);

	int getID() const;

	std::string getInsideWorldName() const;

	Door *getConnectedDoor(Door *door);

private:
	World *outsideWorld;
	World *insideWorld;
	std::string insideWorldName;
	int buildingID;

	std::map<sf::Vector2i, bool> windows;
	std::map<sf::Vector2i, Door> doors;

	sf::IntRect bounds;

	Door *getDoorByID(int doorID, World *doorWorld);
};


#endif
