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
    mHighlightManager(mBus, 7),
    mModManager(mBus)
{
	mBus.addMessageSubscriber<SetVoxelMessage>(*this);
	mBus.addMessageSubscriber<RegionDeliverMessage>(*this);
	mBus.addMessageSubscriber<ChunkDeliverMessage>(*this);
	mBus.addMessageSubscriber<ChunkHighlightedMessage>(*this);
	mBus.addMessageSubscriber<ChunkDehighlightedMessage>(*this);
	mBus.addMessageSubscriber<RegionDeletedMessage>(*this);
}

Universe::~Universe()
{
	mBus.removeMessageSubscriber<SetVoxelMessage>(*this);
	mBus.removeMessageSubscriber<RegionDeliverMessage>(*this);
	mBus.removeMessageSubscriber<ChunkDeliverMessage>(*this);
	mBus.removeMessageSubscriber<ChunkHighlightedMessage>(*this);
	mBus.removeMessageSubscriber<ChunkDehighlightedMessage>(*this);
	mBus.removeMessageSubscriber<RegionDeletedMessage>(*this);
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
    mBus.sendMessage(LogMessage(std::string("saving modifications to disk for all regions"), "file", LogLevel::VERB));
    mModManager.saveMods();
}

void Universe::handleMessage(const SetVoxelMessage& received)
{
    VoxelCoord coordinate;
    VoxelType type;

    std::tie(coordinate, type) = received.data;

    bool succeeded = mStandardWorld.setVoxelType(coordinate, type);

    if(succeeded)
    {
        mBus.sendMessage<VoxelSetMessage>(VoxelSetMessage(coordinate, type));
        mModManager.setMod(voxelToChunk(coordinate), voxelToChunkVoxel(coordinate), type);
    }
}

void Universe::handleMessage(const RegionDeliverMessage& received)
{
    RegionCoord coordinate;
    Region region;

    std::tie(coordinate, region) = received.data;

    mStandardWorld.addRegion(coordinate, region);
    std::cout << "region created: " << glm::to_string((glm::ivec2)coordinate) << "\n";
}

void Universe::handleMessage(const ChunkHighlightedMessage& received)
{
    mBus.sendMessage(ChunkRequestedMessage(received.data));
}

void Universe::handleMessage(const ChunkDehighlightedMessage& received)
{
    mModManager.recordTimestamp(std::get<0>(received.data), 0);
    bool regionDeleted = mStandardWorld.removeChunk(std::get<0>(received.data));
    mBus.sendMessage(ChunkDeletedMessage(received.data));
    
    if(regionDeleted)
    {
        mBus.sendMessage(RegionDeletedMessage(chunkToRegion(std::get<0>(received.data))));
        std::cout << "region deleted: " << glm::to_string(glm::ivec2(chunkToRegion(std::get<0>(received.data)))) << "\n";
    }
}

void Universe::handleMessage(const ChunkDeliverMessage& received)
{
    ChunkCoord coordinate;
    Chunk chunk;

    std::tie(coordinate, chunk) = std::move(received.data);

    mStandardWorld.addChunk(coordinate, chunk);
}

void Universe::handleMessage(const RegionDeletedMessage& received)
{
    mBus.sendMessage(LogMessage("saving modifications to disk for region" + glm::to_string((glm::ivec2)std::get<0>(received.data)), "file", LogLevel::VERB));
    mModManager.saveMods(std::get<0>(received.data));
}

WorldInterface& Universe::getWorldInterface()
{
    return mWorldInterface;
}
