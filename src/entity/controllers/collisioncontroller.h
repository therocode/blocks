#pragma once
#include "blockstd.h"
#include "entitycontroller.h"
#include "../entitymessages.h"

class CollisionController : public EntityController,
                            public fea::MessageReceiver<EntityMoveRequestedMessage>
{
	public:
		struct AABB
		{
			float x, y, z;
			float width = 1.0f, height = 1.0f, depth = 1.0f;
		};
        CollisionController(fea::MessageBus& bus, WorldInterface& worldInterface);
        virtual void inspectEntity(fea::WeakEntityPtr entity) override;
        void onFrame() override;
        void handleMessage(const EntityMoveRequestedMessage& message) override;
        virtual void removeEntity(fea::EntityId id);
    private:
        bool checkIfOnGround(fea::EntityPtr entity);
		float sweepAABB(const AABB a, const AABB b, const glm::vec3 v, glm::vec3& n);
        bool AABBAABB(const AABB a, const AABB b);
};
