#pragma once
#include "entitycontroller.hpp"
#include "../../application/applicationmessages.hpp"
#include "../entitymessages.hpp"
#include "../../utilities/glm.hpp"
#include "../../utilities/geomath.hpp"
#include "../../world/worlddefines.hpp"

class CollisionController : public EntityController,
                            public fea::MessageReceiver<FrameMessage,
                                                        EntityMoveRequestedMessage>
{
	public:
        CollisionController(fea::MessageBus& bus, GameInterface& gameInterface);
        bool keepEntity(fea::WeakEntityPtr entity) const override;
        void handleMessage(const FrameMessage& message) override;
        void handleMessage(const EntityMoveRequestedMessage& message) override;
		bool AABBOnGround(WorldId worldId, AABB a);
    private:
        ///Returns true if aabb hits any block in world. Should be moved to worldInterface in the future.
        bool testAABBWorld(WorldId worldId, const AABB& a) const;
        ///Moves an AABB out from nearby blocks. doesn't push it out more than maxMove
        glm::vec3 pushOutFromBlocks(WorldId worldId, const AABB& _a, float maxMove = 0.1f);
        ///sweep tests an AABB for blocks nearby. returns a value between 0 and 1.0f, 
        ///which is the maximum time the aabb can move before hitting a block, on outnormal.
		float sweepAroundAABB(WorldId worldId, const AABB _a, glm::vec3 velocity, glm::ivec3& outnormal, VoxelCoord& hitBlock, const glm::vec3 ignoreAxis = glm::vec3(0));
        bool checkIfOnGround(fea::EntityPtr entity);
        fea::MessageBus& mBus;
        GameInterface& mGameInterface;
};
