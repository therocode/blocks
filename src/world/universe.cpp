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
    mChunkProvider(mBus, mStandardWorld)
{
	mBus.addMessageSubscriber<SetVoxelMessage>(*this);
	mBus.addMessageSubscriber<RegionDeliverMessage>(*this);
	mBus.addMessageSubscriber<ChunkDeliverMessage>(*this);
}

Universe::~Universe()
{
	mBus.removeMessageSubscriber<SetVoxelMessage>(*this);
	mBus.removeMessageSubscriber<RegionDeliverMessage>(*this);
	mBus.removeMessageSubscriber<ChunkDeliverMessage>(*this);
}

void Universe::setup()
{
    setSimplexSeed(823);
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
}

void Universe::handleMessage(const SetVoxelMessage& received)
{
    VoxelCoord coordinate;
    VoxelType type;

    std::tie(coordinate, type) = received.data;

    ChunkCoord chunkCoord = voxelToChunk(coordinate);
    ChunkVoxelCoord voxelCoord = voxelToChunkVoxel(coordinate);
    
    //if(standardWorld.getLandscape().chunkIsLoaded(chunkCoord))
    //{
    //    standardWorld.getLandscape().getChunk(chunkCoord).setVoxelType(voxelCoord.x, voxelCoord.y, voxelCoord.z, type);

    //    mBus.sendMessage<VoxelSetMessage>(VoxelSetMessage(chunkCoord, voxelCoord, type));
    //}
}

void Universe::handleMessage(const RegionDeliverMessage& received)
{
    RegionCoord coordinate;
    Region region;

    std::tie(coordinate, region) = std::move(received.data);

    mStandardWorld.addRegion(coordinate, region);
}

void Universe::handleMessage(const ChunkDeliverMessage& received)
{
    ChunkCoord coordinate;
    Chunk chunk;

    std::tie(coordinate, chunk) = std::move(received.data);

    mStandardWorld.addChunk(coordinate, chunk);
}

WorldInterface& Universe::getWorldInterface()
{
    return mWorldInterface;
}
