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
        ///Returns true if aabb hits any block in world. Should be moved to worldInterface in the future.
        bool testAABBWorld(const AABB& a) const;
        ///Moves an AABB out from nearby blocks. doesn't push it out more than maxMove
        glm::vec3 pushOutFromBlocks(const AABB& _a, float maxMove = 0.1f);
        ///sweep tests an AABB for blocks nearby. returns a value between 0 and 1.0f, 
        ///which is the maximum time the aabb can move before hitting a block, on outnormal.
		float sweepAroundAABB(const AABB _a, glm::vec3 velocity, glm::ivec3& outnormal, VoxelWorldCoordinate& hitBlock, const glm::vec3 ignoreAxis = glm::vec3(0));
        bool checkIfOnGround(fea::EntityPtr entity);
        fea::MessageBus& mBus;
};
