#include "movementcontroller.h"
#include "moveaction.h"
#include <iostream>
        
MovementController::MovementController(fea::MessageBus& bus, WorldInterface& worldInterface) : EntityController(bus, worldInterface)
{
    mBus.addMessageSubscriber<EntityJumpMessage>(*this);
}

MovementController::~MovementController()
{
    mBus.removeMessageSubscriber<EntityJumpMessage>(*this);
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
        bool backwards = false;

        //if(onGround)
       // {
            if(action == MoveAction::WALKING)
                propellSpeed = entity->getAttribute<float>("walk_speed");
            else if(action == MoveAction::BACKING)
            {
                propellSpeed = entity->getAttribute<float>("walk_speed");
                backwards = true;
            }
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

            if(backwards)
                targetVel *= -1.0f;

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
			if(!onGround)
			{
				glm::vec2 dir;
				if(glm::length2(dir)!= 0)
				{
					dir = glm::normalize(glm::vec2(direction));
				}else
				{
					dir = glm::vec2();
				}
				glm::vec2 vel;
				if(glm::length2(currentVel)!= 0)
				{
					vel = glm::normalize(glm::vec2(currentVel));
				}else
				{
					vel = glm::vec2();
				}
				float d = glm::dot(dir, vel);
				d =  1.01f - (d+1.f)/2.f;
				acc *= d * 0.2f;
            }
			entity->setAttribute<glm::vec3>("acceleration", acc);
        // }
        // else
        // {
            // if(action != MoveAction::STANDING)
            // {
                //hax aircontrol
            // }
           // entity->setAttribute<glm::vec3>("acceleration", glm::vec3());
        // }
    }
}

void MovementController::handleMessage(const EntityJumpMessage& received)
{
    size_t id;

    std::tie(id) = received.data;

    float jumpStrength = mEntities.at(id).lock()->getAttribute<float>("jump_strength");

    if(mEntities.at(id).lock()->getAttribute<bool>("on_ground"))
    {
        mBus.sendMessage<PhysicsImpulseMessage>(PhysicsImpulseMessage(id, glm::vec3(0.0f, jumpStrength / 8.0f, 0.0f)));
        mBus.sendMessage<EntityOnGroundMessage>(EntityOnGroundMessage(id, false));
		mEntities.at(id).lock()->setAttribute<bool>("on_ground", false);
    }
}
