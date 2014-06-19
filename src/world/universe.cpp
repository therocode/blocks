#include "universe.h"
#include "../entity/controllers/playercontroller.h"
#include "../entity/controllers/physicscontroller.h"
#include "../entity/controllers/collisioncontroller.h"
#include "../entity/controllers/gfxcontroller.h"
#include "../entity/controllers/movementcontroller.h"
#include "utilities/simplexnoise.h"

Universe::Universe(fea::MessageBus& messageBus) 
:   mBus(messageBus),
	mEntitySystem(messageBus),
	mWorldInterface(mStandardWorld, mEntitySystem),
    mRegionProvider(mBus),
    mChunkProvider(mBus, mStandardWorld, mModManager),
    mHighlightManager(mBus, 17),
    mModManager(mBus),
    mLodManager(mBus)
{
	mBus.addSubscriber<SetVoxelMessage>(*this);
	mBus.addSubscriber<RegionDeliverMessage>(*this);
	mBus.addSubscriber<ChunkDeliverMessage>(*this);
	mBus.addSubscriber<ChunkHighlightedMessage>(*this);
	mBus.addSubscriber<ChunkDehighlightedMessage>(*this);
	mBus.addSubscriber<RegionDeletedMessage>(*this);
}

Universe::~Universe()
{
	mBus.removeSubscriber<SetVoxelMessage>(*this);
	mBus.removeSubscriber<RegionDeliverMessage>(*this);
	mBus.removeSubscriber<ChunkDeliverMessage>(*this);
	mBus.removeSubscriber<ChunkHighlightedMessage>(*this);
	mBus.removeSubscriber<ChunkDehighlightedMessage>(*this);
	mBus.removeSubscriber<RegionDeletedMessage>(*this);
}

void Universe::setup()
{
    mEntitySystem.setup();

	mEntitySystem.addController(std::unique_ptr<EntityController>(new PlayerController(mBus, mWorldInterface)));
	mEntitySystem.addController(std::unique_ptr<EntityController>(new PhysicsController(mBus, mWorldInterface)));
	mEntitySystem.addController(std::unique_ptr<EntityController>(new CollisionController(mBus, mWorldInterface)));
	mEntitySystem.addController(std::unique_ptr<EntityController>(new MovementController(mBus, mWorldInterface)));
	mEntitySystem.addController(std::unique_ptr<EntityController>(new GfxController(mBus, mWorldInterface)));
}

void Universe::update()
{
	mEntitySystem.update();
}

void Universe::destroy()
{
    mEntitySystem.destroy();
    mBus.send(LogMessage{std::string("saving modifications to disk for all regions"), "file", LogLevel::VERB});
    mModManager.saveMods();
}

void Universe::handleMessage(const SetVoxelMessage& received)
{
    VoxelCoord coordinate = received.voxel;
    VoxelType type = received.type;

    bool succeeded = mStandardWorld.setVoxelType(coordinate, type);

    if(succeeded)
    {
        mBus.send<VoxelSetMessage>(VoxelSetMessage{coordinate, type});
        mModManager.setMod(voxelToChunk(coordinate), voxelToChunkVoxel(coordinate), type);
    }
}

void Universe::handleMessage(const RegionDeliverMessage& received)
{
    RegionCoord coordinate = received.coordinate;
    Region region = received.newRegion;

    mStandardWorld.addRegion(coordinate, region);
    std::cout << "region created: " << glm::to_string((glm::ivec2)coordinate) << "\n";
}

void Universe::handleMessage(const ChunkHighlightedMessage& received)
{
    mBus.send(ChunkRequestedMessage{received.coordinate});
}

void Universe::handleMessage(const ChunkDehighlightedMessage& received)
{
    mModManager.recordTimestamp(received.coordinate, 0);
    bool regionDeleted = mStandardWorld.removeChunk(received.coordinate);
    mBus.send(ChunkDeletedMessage{received.coordinate});
    
    if(regionDeleted)
    {
        mBus.send(RegionDeletedMessage{chunkToRegion(received.coordinate)});
        std::cout << "region deleted: " << glm::to_string(glm::ivec2(chunkToRegion(received.coordinate))) << "\n";
    }
}

void Universe::handleMessage(const ChunkDeliverMessage& received)
{
    ChunkCoord coordinate = received.coordinate;
    Chunk chunk = received.chunk;

    mStandardWorld.addChunk(coordinate, chunk);
}

void Universe::handleMessage(const RegionDeletedMessage& received)
{
    mBus.send(LogMessage{"saving modifications to disk for region" + glm::to_string((glm::ivec2)received.coordinate), "file", LogLevel::VERB});
    mModManager.saveMods(received.coordinate);
}

WorldInterface& Universe::getWorldInterface()
{
    return mWorldInterface;
}
