#include "universe.h"
#include "../entity/controllers/playercontroller.h"
#include "../entity/controllers/physicscontroller.h"
#include "../entity/controllers/collisioncontroller.h"
#include "../entity/controllers/gfxcontroller.h"
#include "../entity/controllers/movementcontroller.h"
#include "utilities/simplexnoise.h"

	Universe::Universe(fea::MessageBus& messageBus) 
:   bus(messageBus),
	standardWorld(messageBus),
	entitySystem(messageBus),
	worldInterface(standardWorld, entitySystem)
{
	bus.addMessageSubscriber<PlayerEntersChunkMessage>(*this);
	bus.addMessageSubscriber<SetVoxelMessage>(*this);
}

Universe::~Universe()
{
	bus.removeMessageSubscriber<PlayerEntersChunkMessage>(*this);
	bus.removeMessageSubscriber<SetVoxelMessage>(*this);
}

void Universe::setup()
{
    setSimplexSeed(823);
    entitySystem.setup();
	standardWorld.initialise();

	entitySystem.addController(std::unique_ptr<EntityController>(new PlayerController(bus, worldInterface)));
	entitySystem.addController(std::unique_ptr<EntityController>(new PhysicsController(bus, worldInterface)));
	entitySystem.addController(std::unique_ptr<EntityController>(new CollisionController(bus, worldInterface)));
	entitySystem.addController(std::unique_ptr<EntityController>(new MovementController(bus, worldInterface)));
	entitySystem.addController(std::unique_ptr<EntityController>(new GfxController(bus, worldInterface)));
}

void Universe::update()
{
	entitySystem.update();
}

void Universe::destroy()
{
    entitySystem.destroy();
}

void Universe::handleMessage(const PlayerEntersChunkMessage& received)
{
    size_t playerId;
    ChunkCoordinate chunkCoordinate;

	std::tie(playerId, chunkCoordinate) = received.data;

    standardWorld.highlightChunk(playerId, chunkCoordinate);
}

void Universe::handleMessage(const SetVoxelMessage& received)
{
    VoxelWorldCoordinate coordinate;
    VoxelType type;

    std::tie(coordinate, type) = received.data;

    ChunkCoordinate chunkCoord = worldToChunkInt(coordinate.x, coordinate.y, coordinate.z);
    VoxelCoordinate voxelCoord = worldToChunkVoxel(coordinate.x, coordinate.y, coordinate.z);
    
    //if(standardWorld.getLandscape().chunkIsLoaded(chunkCoord))
    //{
    //    standardWorld.getLandscape().getChunk(chunkCoord).setVoxelType(voxelCoord.x, voxelCoord.y, voxelCoord.z, type);

    //    bus.sendMessage<VoxelSetMessage>(VoxelSetMessage(chunkCoord, voxelCoord, type));
    //}
}

WorldInterface& Universe::getWorldInterface()
{
    return worldInterface;
}

