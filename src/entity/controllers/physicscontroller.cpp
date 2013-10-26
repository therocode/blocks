#include "physicscontroller.h"
#include "../../blockstd.h"
#include "physicstype.h"

PhysicsController::PhysicsController(fea::MessageBus& bus, WorldInterface& worldInterface) : EntityController(bus, worldInterface), gravityConstant(-0.001f)
{
	mTimer.start();
	accumulator = 0;
    mBus.addMessageSubscriber<GravityRequestedMessage>(*this);
    mBus.addMessageSubscriber<PhysicsImpulseMessage>(*this);
}

PhysicsController::~PhysicsController()
{
    mBus.removeMessageSubscriber<GravityRequestedMessage>(*this);
    mBus.removeMessageSubscriber<PhysicsImpulseMessage>(*this);
}

void PhysicsController::inspectEntity(fea::WeakEntityPtr entity)
{
    fea::EntityPtr locked = entity.lock();

    if(locked->hasAttribute("position") &&
       locked->hasAttribute("velocity") &&
       locked->hasAttribute("drag") && 
       locked->hasAttribute("physics_type"))
    {
        mEntities.emplace(locked->getId(), entity);
    }
}

void PhysicsController::onFrame()
{
	int dt = mTimer.getDeltaTime();
	if(dt > 100) return;
	dt += accumulator;
	int millisecondsPerStep = 10;
	int steps = dt / millisecondsPerStep;
	accumulator = dt % millisecondsPerStep;
	//printf("dt: %i\n", dt);
	if(steps == 0) return;
	if(steps > 10) steps = 10;
    for(auto wEntity : mEntities)
    {
        glm::vec3 gravityForce;

        fea::EntityPtr entity = wEntity.second.lock();
        if(entity->getAttribute<PhysicsType>("physics_type") == PhysicsType::FALLING)
        {
            gravityForce = glm::vec3(0.0f, gravityConstant, 0.0f);
        }

        glm::vec3 currentPosition = entity->getAttribute<glm::vec3>("position");
        glm::vec3 currentVelocity = entity->getAttribute<glm::vec3>("velocity");
     
		glm::vec3 acceleration = gravityForce;
		glm::vec3 newVelocity = currentVelocity + acceleration;
		glm::vec3 newPosition = currentPosition + newVelocity;
		float drag = entity->getAttribute<float>("drag");
		for(int i = 0; i < steps; i ++)
		{
			newPosition = newPosition + newVelocity;
			newVelocity += acceleration;
			newVelocity *= (1.0f - drag);
		}
        entity->setAttribute<glm::vec3>("velocity", newVelocity);
        mBus.sendMessage<EntityMoveRequestedMessage>(EntityMoveRequestedMessage(entity->getId(), newPosition));
    }
}

void PhysicsController::handleMessage(const GravityRequestedMessage& received)
{
    std::tie(gravityConstant) = received.data;
}

void PhysicsController::handleMessage(const PhysicsImpulseMessage& received)
{
    size_t id;
    glm::vec3 force;

    std::tie(id, force) = received.data;

    auto entity = mEntities.find(id);

    if(entity != mEntities.end())
    {
        entity->second.lock()->addToAttribute<glm::vec3>("velocity", force);
    }
}

void PhysicsController::removeEntity(fea::EntityId id)
{
    mEntities.erase(id);
}
