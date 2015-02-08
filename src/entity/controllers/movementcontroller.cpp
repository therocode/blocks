#include "movementcontroller.hpp"
#include "moveaction.hpp"
#include "physicstype.hpp"

MovementController::MovementController(fea::MessageBus& bus) :
    EntityController(bus)
{
    subscribe(mBus, *this);
}

bool MovementController::keepEntity(fea::WeakEntityPtr entity) const
{
    fea::EntityPtr locked = entity.lock();

    return locked->hasAttribute("walk_speed") &&
           locked->hasAttribute("run_speed") &&
           locked->hasAttribute("move_action") &&
           locked->hasAttribute("move_direction") &&
           locked->hasAttribute("orientation") &&
           locked->hasAttribute("jumping") &&
           locked->hasAttribute("jump_strength") &&
           locked->hasAttribute("physics_type") &&
           locked->hasAttribute("on_ground") &&
           locked->hasAttribute("velocity") &&
           locked->hasAttribute("acceleration");
}

void MovementController::handleMessage(const FrameMessage& received)
{
    int32_t dt = received.deltaTime;

    for(auto wEntity : mEntities)
    {
        float maxAcc = 0.01f;
        fea::EntityPtr entity = wEntity.second.lock();

        MoveAction action = entity->getAttribute<MoveAction>("move_action");
        MoveDirection moveDirection = entity->getAttribute<MoveDirection>("move_direction");
        bool onGround = entity->getAttribute<bool>("on_ground");

        float propellSpeed = 0.0f;
        bool backwards = false;

		if(action == MoveAction::WALKING)
			propellSpeed = entity->getAttribute<float>("walk_speed");
		else if(action == MoveAction::WALKING)
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

		const glm::quat& orientation = entity->getAttribute<glm::quat>("orientation");

		//glm::vec3 forwardDirection;
		//glm::vec3 sideDirection = orientation * glm::vec3((float) moveDirection.getLeftRight(), 0.0f, 0.0f);
		//if(entity->getAttribute<PhysicsType>("physics_type") == PhysicsType::FALLING)
		//{
		//	forwardDirection = orientation *  glm::vec3(0.0f, 0.0f, (float)moveDirection.getForwardBack());
        //    forwardDirection.y = 0.0f;
        //    forwardDirection.z -= 0.00001f;
        //    forwardDirection = glm::normalize(forwardDirection);

        //    sideDirection = orientation * glm::vec3((float) moveDirection.getLeftRight(), 0.0f, 0.0f);
        //    sideDirection.y = 0.0f;
        //    sideDirection.z -= 0.00001f;
        //    sideDirection = glm::normalize(sideDirection);
		//}
        //else
		//{
		//	//forwardDirection = glm::vec3(glm::cos(pitch) * glm::sin(yaw), glm::sin(pitch), glm::cos(pitch) * glm::cos(yaw)) * (float) moveDirection.getForwardBack();
		//	forwardDirection = orientation *  glm::vec3(0.0f, 0.0f, (float)moveDirection.getForwardBack());

        //    sideDirection = orientation * glm::vec3((float) moveDirection.getLeftRight(), 0.0f, 0.0f);
		//}

		//if(moveDirection.getLeftRight() == 0)
		//{
		//	sideDirection = glm::vec3(0.0f, 0.0f, 0.0f);
		//}

		//glm::vec3 direction = (forwardDirection + sideDirection);
        
        glm::vec3 direction = orientation * glm::vec3((float) moveDirection.getLeftRight(), 0.0f, (float)moveDirection.getForwardBack());

        PhysicsType physicsType = entity->getAttribute<PhysicsType>("physics_type");
        if(physicsType ==  PhysicsType::FALLING)
        {
            direction.y = 0.0f;
        }

		if(glm::length2(direction) !=0)
			direction = glm::normalize(direction);

		glm::vec3 targetVel;

		if(propellSpeed > 0.0f)
		{
			targetVel = direction * propellSpeed;
		}

		if(backwards)
			targetVel *= -1.0f;

		glm::vec3 currentVel = entity->getAttribute<glm::vec3>("velocity");
		currentVel.y = 0.0f;     //this handler does not do gravity or up/down things

		glm::vec3 acc = (targetVel - currentVel);// / timestep;
		if(entity->getAttribute<PhysicsType>("physics_type") == PhysicsType::FALLING)
		{
			glm::vec2 horizontalAcc = glm::vec2(acc.x, acc.z);
			if(glm::length(horizontalAcc) > maxAcc)
			{
				if(glm::length2(horizontalAcc) != 0)
					horizontalAcc = glm::normalize(horizontalAcc) * maxAcc;
				acc.x = horizontalAcc.x;
				acc.z = horizontalAcc.y;
			}
			if(!onGround)
			{
				glm::vec2 dir = glm::vec2(direction);
				if(glm::length2(dir)!= 0)
				{
					dir = glm::normalize(dir);
				}else
				{
					dir = glm::vec2(0.f, 0.f);
				}
				glm::vec2 vel = glm::vec2(currentVel);
				if(glm::length2(vel)!= 0)
				{
					vel = glm::normalize(vel);
				}else
				{
					vel = glm::vec2(0.f, 0.f);
				}
				float d = glm::dot(dir, vel);
				d =  1.5f - (d+1.f)/2.f;
				acc *= d * 0.08f;
			}
		}else
		{
			acc *= 0.01f;
		}
		entity->setAttribute<glm::vec3>("acceleration", acc);

		if(entity->getAttribute<bool>("jumping")){
			float jumpStrength = entity->getAttribute<float>("jump_strength");
			float ySpeed = entity->getAttribute<glm::vec3>("velocity").y;
			if(onGround)
			{
				mBus.send(PhysicsImpulseMessage{entity->getId(), glm::vec3(0.0f, jumpStrength / 8.0f, 0.0f)});
				mBus.send(EntityOnGroundMessage{entity->getId(), false});
				entity->setAttribute<bool>("on_ground", false);
			}
		}
    }
}

void MovementController::handleMessage(const EntityJumpMessage& received)
{
	mEntities.at(received.id).lock()->setAttribute<bool>("jumping", received.jumping);
}
