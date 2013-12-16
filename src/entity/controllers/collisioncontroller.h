#pragma once
#include "blockstd.h"
#include "entitycontroller.h"
#include "../entitymessages.h"
#include "math/mathutils.h"

class CollisionController : public EntityController,
                            public fea::MessageReceiver<EntityMoveRequestedMessage>
{
	public:
        CollisionController(fea::MessageBus& bus, WorldInterface& worldInterface);
        ~CollisionController();
        virtual void inspectEntity(fea::WeakEntityPtr entity) override;
        void onFrame(int dt) override;
        void handleMessage(const EntityMoveRequestedMessage& message) override;
        virtual void removeEntity(fea::EntityId id);
		bool AABBOnGround(AABB a);
    private:
		float sweepAroundAABB(const AABB _a, glm::vec3 velocity, glm::ivec3& outnormal, VoxelWorldCoordinate& hitBlock, const glm::vec3 ignoreAxis = glm::vec3(0));
        bool checkIfOnGround(fea::EntityPtr entity);
        fea::MessageBus& mBus;
};
