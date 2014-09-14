#include "worldholder.hpp"
#include "../entity/controllers/playercontroller.hpp"
#include "../entity/controllers/physicscontroller.hpp"
#include "../entity/controllers/collisioncontroller.hpp"
#include "../entity/controllers/gfxcontroller.hpp"
#include "../entity/controllers/movementcontroller.hpp"
#include <fea/util.hpp>

WorldHolder::WorldHolder(fea::MessageBus& messageBus) 
:   mBus(messageBus),
	mEntitySystem(messageBus),
	mWorldInterface(mWorlds, mEntitySystem),
    mWorldProvider(mBus, mModManager),
    mHighlightManager(mBus, 8),
    mModManager(messageBus),
    mRegionManager(mBus)
{
	mBus.addSubscriber<SetVoxelMessage>(*this);
	mBus.addSubscriber<RegionDeliverMessage>(*this);
	mBus.addSubscriber<ChunkDeliverMessage>(*this);
	mBus.addSubscriber<ChunkHighlightedMessage>(*this);
	mBus.addSubscriber<ChunkDehighlightedMessage>(*this);
	mBus.addSubscriber<RegionDeletedMessage>(*this);

    mWorlds.emplace("default", World(mBus));
    mModManager.setWorldName("default");
}

WorldHolder::~WorldHolder()
{
	mBus.removeSubscriber<SetVoxelMessage>(*this);
	mBus.removeSubscriber<RegionDeliverMessage>(*this);
	mBus.removeSubscriber<ChunkDeliverMessage>(*this);
	mBus.removeSubscriber<ChunkHighlightedMessage>(*this);
	mBus.removeSubscriber<ChunkDehighlightedMessage>(*this);
	mBus.removeSubscriber<RegionDeletedMessage>(*this);
}

void WorldHolder::setup()
{
    mEntitySystem.setup();

	mEntitySystem.addController(std::unique_ptr<EntityController>(new PlayerController(mBus, mWorldInterface)));
	mEntitySystem.addController(std::unique_ptr<EntityController>(new PhysicsController(mBus, mWorldInterface)));
	mEntitySystem.addController(std::unique_ptr<EntityController>(new CollisionController(mBus, mWorldInterface)));
	mEntitySystem.addController(std::unique_ptr<EntityController>(new MovementController(mBus, mWorldInterface)));
	mEntitySystem.addController(std::unique_ptr<EntityController>(new GfxController(mBus, mWorldInterface)));
}

void WorldHolder::update()
{
	mEntitySystem.update();
}

void WorldHolder::destroy()
{
    mEntitySystem.destroy();
    mBus.send(LogMessage{std::string("saving modifications to disk for all regions"), "file", LogLevel::VERB});
    mModManager.saveMods();
}

void WorldHolder::handleMessage(const SetVoxelMessage& received)
{
    VoxelCoord coordinate = received.voxel;
    VoxelType type = received.type;

    bool succeeded = mWorlds.at("default").setVoxelType(coordinate, type);

    if(succeeded)
    {
        mBus.send<VoxelSetMessage>(VoxelSetMessage{coordinate, type});
        mModManager.setMod(voxelToChunk(coordinate), voxelToChunkVoxel(coordinate), type);
    }
}

void WorldHolder::handleMessage(const RegionDeliverMessage& received)
{
    RegionCoord coordinate = received.coordinate;
    Region region = received.newRegion;

    mWorlds.at("default").addRegion(coordinate, region);
    mBus.send(LogMessage{"region created" + glm::to_string((glm::ivec2)coordinate), "landscape", LogLevel::VERB});
}

void WorldHolder::handleMessage(const ChunkHighlightedMessage& received)
{
    mWorlds.at("default").activateChunk(received.coordinate);
    mBus.send(ChunkRequestedMessage{received.coordinate});
}

void WorldHolder::handleMessage(const ChunkDehighlightedMessage& received)
{
    mModManager.recordTimestamp(received.coordinate, 0);
    mWorlds.at("default").deactivateChunk(received.coordinate);
}

void WorldHolder::handleMessage(const ChunkDeliverMessage& received)
{
    ChunkCoord coordinate = received.coordinate;
    Chunk chunk = received.chunk;

    mWorlds.at("default").addChunk(coordinate, chunk);
}

void WorldHolder::handleMessage(const RegionDeletedMessage& received)
{
    mBus.send(LogMessage{"saving modifications to disk for region" + glm::to_string((glm::ivec2)received.coordinate), "file", LogLevel::VERB});
    mModManager.saveMods(received.coordinate);
}

WorldInterface& WorldHolder::getWorldInterface()
{
    return mWorldInterface;
}
