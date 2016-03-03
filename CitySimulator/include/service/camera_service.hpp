#ifndef CITYSIM_CAMERA_SERVICE_HPP
#define CITYSIM_CAMERA_SERVICE_HPP

#include "services.hpp"
#include "world.hpp"

class CameraService : public BaseService {
public:
	CameraService(World &world);

	~CameraService();

	virtual void onEnable() override;

	virtual void onDisable() override;

	void tick(float delta);

	void switchWorld(World &world);

	void setTrackedEntity(EntityID entity);

	void clearPlayerEntity();

	void updateViewSize(unsigned int width, unsigned int height);

	// merci: https://github.com/SFML/SFML/wiki/Source:-Zoom-View-At-(specified-pixel)
	void zoomTo(float delta, const sf::Vector2i &pixel, sf::RenderWindow &window);

private:
	World *world;
	PhysicsComponent *trackedEntity;
	sf::View view;
	float zoom;

	SimpleMovementController *controller;
};
#endif