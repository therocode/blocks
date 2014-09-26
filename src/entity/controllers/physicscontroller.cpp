#include "physicscontroller.hpp"
#include "../../blockstd.hpp"
#include "physicstype.hpp"

PhysicsController::PhysicsController(fea::MessageBus& bus, WorldInterface& worldInterface) : EntityController(bus, worldInterface), gravityConstant(-0.003f)
{
    mTimer.start();
    accumulator = 0;
    subscribe(mBus, *this);
}

void PhysicsController::inspectEntity(fea::WeakEntityPtr entity)
{
    fea::EntityPtr locked = entity.lock();

    if(locked->hasAttribute("position") &&
            locked->hasAttribute("velocity") &&
            locked->hasAttribute("acceleration") &&
            locked->hasAttribute("drag") && 
            locked->hasAttribute("on_ground") && 
            locked->hasAttribute("physics_type"))
    {
        mEntities.emplace(locked->getId(), entity);
    }
}

void PhysicsController::onFrame(int dt)
{
    //int dt = mTimer.getDeltaTime();
    if(dt > 500) return;
    dt += accumulator;
    int millisecondsPerStep = 1000/60;
    int steps = dt / millisecondsPerStep;
    accumulator = dt % millisecondsPerStep;
    //printf("dt: %i\n", dt);
    if(steps == 0) return;
    if(steps > 10) steps = 10;

    //steps = 1; // if steps are used, it turns weird with the movementcontroller

    for(auto wEntity : mEntities)
    {
        glm::vec3 gravityForce;

        fea::EntityPtr entity = wEntity.second.lock();
        if(entity->getAttribute<PhysicsType>("physics_type") == PhysicsType::FALLING)
        {
            gravityForce = glm::vec3(0.0f, gravityConstant, 0.0f);
        }

        glm::vec3 newPosition = entity->getAttribute<glm::vec3>("position");
        glm::vec3 currentPosition = entity->getAttribute<glm::vec3>("position");
        glm::vec3 currentVelocity = entity->getAttribute<glm::vec3>("velocity");

        glm::vec3 acceleration = gravityForce + entity->getAttribute<glm::vec3>("acceleration");
        glm::vec3 newVelocity = currentVelocity + acceleration;
        for(int i = 0; i < steps; i ++)
        {
            newPosition = currentPosition + newVelocity;
            float drag = entity->getAttribute<float>("drag");
            bool onGround = entity->getAttribute<bool>("on_ground");
            newPosition = newPosition + newVelocity;
            newVelocity += acceleration;
            newVelocity *= (1.0f - drag);
            if(onGround){
                newVelocity *= 0.9f; 
            }
        }
        entity->setAttribute<glm::vec3>("velocity", newVelocity);
        mBus.send(EntityMoveRequestedMessage{entity->getId(), newPosition});
    }
}

void PhysicsController::handleMessage(const GravityRequestedMessage& received)
{
    gravityConstant = received.gravityConstant;
}

void PhysicsController::handleMessage(const PhysicsImpulseMessage& received)
{
    auto entity = mEntities.find(received.id);

    if(entity != mEntities.end())
    {
        entity->second.lock()->addToAttribute<glm::vec3>("velocity", received.force / 10.0f);
    }
}

void PhysicsController::removeEntity(fea::EntityId id)
{
    mEntities.erase(id);
}
