#ifndef CITYSIM_AI_HPP
#define CITYSIM_AI_HPP

#include "entity.hpp"
#include "services.hpp"

struct PhysicsComponent;

class EntityBrain
{
public:
	explicit EntityBrain(EntityID e);

	virtual ~EntityBrain()
	{
	}

	virtual void tick(float delta) = 0;

protected:
	EntityID entity;
	PhysicsComponent *phys;

	virtual void onEnable()
	{
	}

	virtual void onDisable()
	{
	}

};

class InputBrain : public EntityBrain
{
public:
	InputBrain(EntityID e);
	~InputBrain();

private:
	void tick(float delta) override;

	SimpleMovementController controller;

};

class AIBrain : public EntityBrain
{
public:
	AIBrain(EntityID e) : EntityBrain(e)
	{
	}

	void tick(float delta) override;
};

#endif
