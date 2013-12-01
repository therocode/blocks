#include "world.h"
#include "../entity/controllers/playercontroller.h"
#include "../entity/controllers/physicscontroller.h"
#include "../entity/controllers/collisioncontroller.h"
#include "../entity/controllers/gfxcontroller.h"
#include "../entity/controllers/movementcontroller.h"
#include "utilities/simplexnoise.h"

	World::World(fea::MessageBus& messageBus) 
:   bus(messageBus),
	standardDimension(messageBus),
	entitySystem(messageBus),
	worldInterface(standardDimension, entitySystem)
{
	bus.addMessageSubscriber<PlayerEntersChunkMessage>(*this);
	bus.addMessageSubscriber<SetVoxelMessage>(*this);
}

World::~World()
{
	bus.removeMessageSubscriber<PlayerEntersChunkMessage>(*this);
	bus.removeMessageSubscriber<SetVoxelMessage>(*this);
}

void World::setup()
{
    setSimplexSeed(823);
    entitySystem.setup();
	standardDimension.initialise();

	entitySystem.addController(std::unique_ptr<EntityController>(new PlayerController(bus, worldInterface)));
	entitySystem.addController(std::unique_ptr<EntityController>(new PhysicsController(bus, worldInterface)));
	entitySystem.addController(std::unique_ptr<EntityController>(new CollisionController(bus, worldInterface)));
	entitySystem.addController(std::unique_ptr<EntityController>(new MovementController(bus, worldInterface)));
	entitySystem.addController(std::unique_ptr<EntityController>(new GfxController(bus, worldInterface)));
}

void World::update()
{
	entitySystem.update();
}

void World::destroy()
{
    entitySystem.destroy();
}

void World::handleMessage(const PlayerEntersChunkMessage& received)
{
    size_t playerId;
    ChunkCoordinate chunkCoordinate;

	std::tie(playerId, chunkCoordinate) = received.data;

    standardDimension.highlightChunk(playerId, chunkCoordinate);
}

void World::handleMessage(const SetVoxelMessage& received)
{
    VoxelWorldCoordinate coordinate;
    VoxelType type;

    std::tie(coordinate, type) = received.data;

    ChunkCoordinate chunkCoord = worldToChunkInt(coordinate.x, coordinate.y, coordinate.z);
    VoxelCoordinate voxelCoord = worldToChunkVoxel(coordinate.x, coordinate.y, coordinate.z);
    
    if(standardDimension.getLandscape().chunkIsLoaded(chunkCoord))
    {
        standardDimension.getLandscape().getChunk(chunkCoord).setVoxelType(voxelCoord.x, voxelCoord.y, voxelCoord.z, type);

        bus.sendMessage<VoxelSetMessage>(VoxelSetMessage(chunkCoord, voxelCoord, type));
    }
}

WorldInterface& World::getWorldInterface()
{
    return worldInterface;
}

