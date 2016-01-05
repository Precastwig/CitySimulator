#include "services.hpp"

CameraService::CameraService(World &world) : world(&world), trackedEntity(nullptr)
{
	float speed = Config::getFloat("debug.movement.camera-speed");
	controller = new SimpleMovementController(CAMERA_ENTITY, speed, speed, speed);
}

CameraService::~CameraService()
{
	delete controller;
}

void CameraService::onEnable()
{
	sf::Vector2f size = static_cast<sf::Vector2f>(Constants::windowSize);

	view.setSize(size);
	view.zoom(Config::getFloat("debug.zoom"));
	view.reset(sf::FloatRect(-size.x / 4, -size.y / 4, size.x, size.y));
	updateWindowView();

	clearPlayerEntity();
}

void CameraService::updateWindowView() const
{
	Locator::locate<RenderService>()->getWindow()->setView(view);
}

void CameraService::onDisable()
{
}

void CameraService::tick(float delta)
{
	if (trackedEntity != nullptr)
	{
		view.setCenter(trackedEntity->getPosition());
		updateWindowView();
	}
	else
	{
		float speed; // todo currently unused
		b2Vec2 movement(controller->tick(delta, speed));
		view.move(movement.x * delta, movement.y * delta);
		updateWindowView();
	}
}

void CameraService::switchWorld(World &world)
{
	Logger::logDebug("Switched camera world");
	this->world = &world;
}

void CameraService::setTrackedEntity(EntityID entity)
{
	EntityService *es = Locator::locate<EntityService>();
	if (es->hasComponent(entity, COMPONENT_PHYSICS))
	{
		trackedEntity = es->getComponent<PhysicsComponent>(entity, COMPONENT_PHYSICS);
		Logger::logDebug(format("Started tracking entity %1%", std::to_string(entity)));

		controller->unregisterListeners();
	}
	else
		Logger::logWarning(
				format("Could not track entity %1% as it doesn't have a physics component", std::to_string(entity)));
}

void CameraService::clearPlayerEntity()
{
	trackedEntity = nullptr;
	controller->registerListeners();
}