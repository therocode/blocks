#include "movementcontroller.hpp"
#include "moveaction.hpp"

MovementController::MovementController(fea::MessageBus& bus, WorldInterface& worldInterface) : EntityController(bus, worldInterface)
{
    subscribe(mBus, *this);
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

void MovementController::onFrame(int dt)
{

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

		float pitch = entity->getAttribute<float>("pitch");
		float yaw = entity->getAttribute<float>("yaw");
		glm::vec3 forwardDirection;
		if(entity->getAttribute<PhysicsType>("physics_type") == PhysicsType::FALLING)
		{
			forwardDirection = glm::vec3(glm::sin(yaw), 0.0f, glm::cos(yaw)) * (float) moveDirection.getForwardBack();
		}else
		{
			forwardDirection = glm::vec3(glm::cos(pitch) * glm::sin(yaw), glm::sin(pitch), glm::cos(pitch) * glm::cos(yaw)) * (float) moveDirection.getForwardBack();
		}

		glm::vec3 sideDirection = glm::vec3(glm::sin(yaw + glm::radians(90.0f * (float)moveDirection.getLeftRight())), 0.0f,glm::cos(yaw + glm::radians(90.0f * (float)moveDirection.getLeftRight())));

		if(moveDirection.getLeftRight() == 0)
		{
			sideDirection = glm::vec3(0.0f, 0.0f, 0.0f);
		}

		glm::vec3 direction = (forwardDirection + sideDirection);
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
