#ifndef CITYSIMULATOR_ENTITY_HPP
#define CITYSIMULATOR_ENTITY_HPP

#define INVALID_ENTITY (-1)
#define CAMERA_ENTITY (-2)

typedef int EntityID;

#include <boost/smart_ptr/shared_ptr.hpp>
#include <vector>
#include <Box2D/Common/b2Math.h>
#include <Box2D/Dynamics/b2Body.h>
#include "animation.hpp"
#include "config.hpp"
#include "constants.hpp"

class b2World;


// component-entity-systems

struct EntityIdentifier
{
	EntityID id;
	EntityType type;

	EntityIdentifier() : type(ENTITY_UNKNOWN), id(INVALID_ENTITY)
	{
	}

	EntityIdentifier(EntityID id, EntityType type) : type(type), id(id)
	{
	}
};

enum ComponentType
{
	COMPONENT_PHYSICS = 1 << 0,
	COMPONENT_RENDER = 1 << 1,
	COMPONENT_INPUT = 1 << 2,

	COMPONENT_UNKNOWN = 0
};

// components
struct BaseComponent
{
	virtual ~BaseComponent()
	{
	}

	virtual void reset()
	{
	}
};

struct RenderComponent : BaseComponent
{
	void reset() override;

	Animator anim;
};

class Brain;

struct InputComponent : BaseComponent
{
	void reset() override;

	boost::shared_ptr<Brain> brain;
};

struct PhysicsComponent : BaseComponent
{
	void reset() override;

	inline sf::Vector2f getTilePosition() const
	{
		return Utils::fromB2Vec<float>(body->GetPosition());
	}

	inline sf::Vector2f getPosition() const
	{
		b2Vec2 pos(body->GetPosition());
		return Utils::toPixel(Utils::fromB2Vec<float>(pos));
	}

	inline sf::Vector2f getVelocity() const
	{
		b2Vec2 v = body->GetLinearVelocity();
		return Utils::fromB2Vec<float>(v);
	}

	inline sf::Vector2f getLastVelocity() const
	{
		return Utils::fromB2Vec<float>(lastVelocity);
	}

	inline void setVelocity(const sf::Vector2f &velocity)
	{
		body->SetLinearVelocity(Utils::toB2Vec(velocity));
	}

	inline bool isStopped()
	{
		return Math::lengthSquared(getVelocity()) < 1;
	}

	inline bool isSteering()
	{
		return steering.x != 0.f || steering.y != 0.f;
	}

	float maxSpeed;
	float damping;

	b2Body *body;
	b2World *bWorld;
	b2Vec2 lastVelocity;

	b2Vec2 steering;
};

class EntityService;

// systems
class System
{
public:
	explicit System(int componentMask) : mask(componentMask)
	{
	}

	virtual ~System()
	{
	}

	void tick(EntityService *es, float dt);

	void render(EntityService *es, sf::RenderWindow &window);

	virtual void tickEntity(EntityService *es, EntityID e, float dt) = 0;

	virtual void renderEntity(EntityService *es, EntityID e, sf::RenderWindow &window)
	{
	}

protected:
	int mask;
};

class RenderSystem : public System
{
public:
	RenderSystem() : System(COMPONENT_PHYSICS | COMPONENT_RENDER)
	{
	}

	void tickEntity(EntityService *es, EntityID e, float dt) override;

	void renderEntity(EntityService *es, EntityID e, sf::RenderWindow &window) override;
};

class InputSystem : public System
{
public:
	InputSystem() : System(COMPONENT_INPUT)
	{
	}

	void tickEntity(EntityService *es, EntityID e, float dt) override;
};

class PhysicsSystem : public System
{
public:
	explicit PhysicsSystem() : System(COMPONENT_PHYSICS)
	{
	}

	void tickEntity(EntityService *es, EntityID e, float dt) override;
};

#endif