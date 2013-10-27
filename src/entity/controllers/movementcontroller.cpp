#include "movementcontroller.h"
#include "moveaction.h"
#include <iostream>
        
MovementController::MovementController(fea::MessageBus& bus, WorldInterface& worldInterface) : EntityController(bus, worldInterface)
{
}

void MovementController::inspectEntity(fea::WeakEntityPtr entity)
{
    fea::EntityPtr locked = entity.lock();

    if(locked->hasAttribute("walk_speed") &&
       locked->hasAttribute("run_speed") &&
       locked->hasAttribute("move_action") &&
       locked->hasAttribute("on_ground") &&
       locked->hasAttribute("pitch") &&
       locked->hasAttribute("yaw") &&
       locked->hasAttribute("velocity") &&
       locked->hasAttribute("acceleration"))
    {
        mEntities.emplace(locked->getId(), entity);
    }
}
 
void MovementController::removeEntity(fea::EntityId id)
{
    mEntities.erase(id);
}

void MovementController::onFrame()
{
    
    for(auto wEntity : mEntities)
    {
        float maxAcc = 0.01f;
        fea::EntityPtr entity = wEntity.second.lock();

        MoveAction action = entity->getAttribute<MoveAction>("move_action");
        bool onGround = entity->getAttribute<bool>("on_ground");

        float propellSpeed = 0.0f;

        if(onGround)
        {
            if(action == MoveAction::WALKING)
                propellSpeed = entity->getAttribute<float>("walk_speed");
            else if(action == MoveAction::RUNNING)
                propellSpeed = entity->getAttribute<float>("run_speed");
            else if(action == MoveAction::STANDING)
            {
                propellSpeed = 0.0f;
                maxAcc = 0.0001f;
            }

            float pitch = entity->getAttribute<float>("pitch");
            float yaw = entity->getAttribute<float>("yaw");

            glm::vec3 direction = glm::vec3(glm::sin(yaw), 0.0f,glm::cos(yaw));

            glm::vec3 targetVel;

            if(propellSpeed > 0.0f)
            {
                targetVel = glm::normalize(direction) * propellSpeed;
            }

            glm::vec3 currentVel = entity->getAttribute<glm::vec3>("velocity");
            currentVel.y = 0.0f;     //this handler does not do gravity or up/down things

            glm::vec3 acc = (targetVel - currentVel);// / timestep;
			glm::vec2 horizontalAcc = glm::vec2(acc.x, acc.z);
			if(glm::length(horizontalAcc) > maxAcc)
			{
				horizontalAcc = glm::normalize(horizontalAcc) * maxAcc;
				acc.x = horizontalAcc.x;
				acc.z = horizontalAcc.y;
			}

            entity->setAttribute<glm::vec3>("acceleration", acc);
        }
        else
        {
            if(action != MoveAction::STANDING)
            {
                //hax a bit of air control for the player
            }
            entity->setAttribute<glm::vec3>("acceleration", glm::vec3());
        }
    }
}
