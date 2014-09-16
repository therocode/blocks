#include "worldholder.hpp"
#include <fea/util.hpp>
#include "../application/applicationmessages.hpp"

WorldHolder::WorldHolder(fea::MessageBus& messageBus, EntitySystem& entitySystem) 
:   mBus(messageBus),
	mWorldInterface(mWorlds, entitySystem)
{
	mBus.addSubscriber<SetVoxelMessage>(*this);
	mBus.addSubscriber<RegionDeliverMessage>(*this);
	mBus.addSubscriber<ChunkDeliverMessage>(*this);

    mWorldIds.emplace("default", 0);
    mWorlds.emplace(0, World(mBus, 0, "default", {{Ranges::MIN, Ranges::MAX}, {Ranges::MIN, Ranges::MAX}, {Ranges::MIN, Ranges::MAX}}));
}

WorldHolder::~WorldHolder()
{
	mBus.removeSubscriber<SetVoxelMessage>(*this);
	mBus.removeSubscriber<RegionDeliverMessage>(*this);
	mBus.removeSubscriber<ChunkDeliverMessage>(*this);
}

void WorldHolder::handleMessage(const SetVoxelMessage& received)
{
    bool succeeded = mWorlds.at(received.worldId).setVoxelType(received.voxel, received.type);

    if(succeeded)
    {
        mBus.send<VoxelSetMessage>(VoxelSetMessage{received.voxel, received.type});
    }
}

void WorldHolder::handleMessage(const RegionDeliverMessage& received)
{
    RegionCoord coordinate = received.coordinate;
    Region region = received.newRegion;

    mWorlds.at(0).deliverRegion(coordinate, region); //BLAPP
    mBus.send(LogMessage{"region created" + glm::to_string((glm::ivec2)coordinate), "landscape", LogLevel::VERB});
}

void WorldHolder::handleMessage(const ChunkDeliverMessage& received)
{
    ChunkCoord coordinate = received.coordinate;
    Chunk chunk = received.chunk;

    mWorlds.at(0).deliverChunk(coordinate, chunk);//BLAPP

    uint64_t timestamp = 0; //get proper timestamp later
    mBus.send(ChunkLoadedMessage{chunk, timestamp}); //the now fully initialised chunk is announced to the rest of the game. should it be here?
}

WorldInterface& WorldHolder::getWorldInterface()
{
    return mWorldInterface;
}
