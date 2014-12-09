#include "clientworld.hpp"

void ClientWorld::handleMessage(const ClientChunksDeliveredMessage& received)
{
}

void ClientWorld::updateVoxelLookAt()
{
	glm::vec3 direction = glm::vec3(glm::cos(mPitch) * glm::sin(mYaw), glm::sin(mPitch), glm::cos(mPitch) * glm::cos(mYaw));

	VoxelCoord block;
	uint32_t face = 0;
	bool f = RayCaster::getVoxelAtRay(mLocalChunks, mPosition + glm::vec3(0, 0.6f, 0), direction, 200.f, face, block);

    mBus.send(FacingBlockMessage{block});
}
