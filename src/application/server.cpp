#include "server.hpp"
#include "../networking/packages.hpp"
#include "../networking/clientconnection.hpp"
#include "../networking/clientconnectionlistener.hpp"
#include "../entity/controllers/playercontroller.hpp"
#include "../entity/controllers/physicscontroller.hpp"
#include "../entity/controllers/collisioncontroller.hpp"
#include "../entity/controllers/gfxcontroller.hpp"
#include "../entity/controllers/movementcontroller.hpp"

Server::Server() : 
	mEntitySystem(mBus),
    mWorlds(mBus, mEntitySystem),
    mWorldProvider(mBus),
    mLogger(mBus, LogLevel::VERB),
    mScriptHandler(mBus, mWorlds.getWorldInterface()),
    mLogName("server")
{
    subscribe(mBus, *this);
}

Server::~Server()
{
    mBus.send<LogMessage>(LogMessage{"Server destroyed", mLogName, LogLevel::INFO});
}

void Server::setup()
{
    mScriptHandler.setup();

    mEntitySystem.setup();
	mEntitySystem.addController(std::unique_ptr<EntityController>(new PlayerController(mBus, mWorlds.getWorldInterface())));
	mEntitySystem.addController(std::unique_ptr<EntityController>(new PhysicsController(mBus, mWorlds.getWorldInterface())));
	mEntitySystem.addController(std::unique_ptr<EntityController>(new CollisionController(mBus, mWorlds.getWorldInterface())));
	mEntitySystem.addController(std::unique_ptr<EntityController>(new MovementController(mBus, mWorlds.getWorldInterface())));
	mEntitySystem.addController(std::unique_ptr<EntityController>(new GfxController(mBus, mWorlds.getWorldInterface())));

    WorldLoader mWorldLoader;

    mWorldLoader.loadWorldFile("data/worlds/default.json");

    if(!mWorldLoader.hasError())
    {
        for(const auto& worldParameters : mWorldLoader.getLoadedWorlds())
        {
            mWorlds.addWorld(worldParameters);
        }
    }
    else
    {
        mBus.send<LogMessage>(LogMessage{"World loading error: " + mWorldLoader.getErrorString(), mLogName, LogLevel::ERR});
    }
    

    mFPSController.setMaxFPS(60);
    mBus.send<LogMessage>(LogMessage{"Server initialised and ready to go", mLogName, LogLevel::INFO});
    mBus.send<GameStartMessage>(GameStartMessage{});
}

fea::MessageBus& Server::getBus()
{
    return mBus;
}

void Server::doLogic()
{
    //mFPSController.frameBegin();
    checkForDisconnectedClients();

    for(auto& client : mClients)
    {
        fetchClientData(client.second);
    }

    mBus.send<FrameMessage>(FrameMessage{true});

	mEntitySystem.update();

    for(auto& client : mClients)
    {
        client.second->flush();
    }

    pollNewClients();

    mFPSController.frameEnd();
}

void Server::handleMessage(const FatalMessage& received)
{
    mBus.send<LogMessage>(LogMessage{received.message, mLogName, LogLevel::ERR});
    exit(4);
}

void Server::handleMessage(const ChunkLoadedMessage& received)
{
    const ChunkCoord& coordinate = received.chunk.getLocation();
    const Chunk& chunk = received.chunk;

    VoxelTypeData typeData = chunk.getVoxelTypeData();

    std::shared_ptr<BasePackage> chunkLoadedPackage(new ChunkLoadedPackage(coordinate, typeData.mRleSegmentIndices, typeData.mRleSegments));

    for(auto& client : mClients)
    {
        client.second->enqueuePackage(chunkLoadedPackage);
    }
}

void Server::handleMessage(const ChunkDeletedMessage& received)
{
    std::shared_ptr<BasePackage> chunkDeletedPackage(new ChunkDeletedPackage{received.coordinate});
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(chunkDeletedPackage);
    }
}

void Server::handleMessage(const AddGfxEntityMessage& received)
{
    std::shared_ptr<BasePackage> gfxEntityAddedPackage(new GfxEntityAddedPackage(received.id, received.position));
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(gfxEntityAddedPackage);
    }

    graphicsEntities.insert(received.id); //temphack
}

void Server::handleMessage(const MoveGfxEntityMessage& received)
{
    std::shared_ptr<BasePackage> gfxEntityMovedPackage(new GfxEntityMovedPackage(received.id, received.position));
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(gfxEntityMovedPackage);
    }
}

void Server::handleMessage(const RotateGfxEntityMessage& received)
{
    std::shared_ptr<BasePackage> gfxEntityRotatedPackage(new GfxEntityRotatedPackage(received.id, received.pitch, received.yaw));
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(gfxEntityRotatedPackage);
    }
}

void Server::handleMessage(const RemoveGfxEntityMessage& received)
{
    std::shared_ptr<BasePackage> gfxEntityRemovedPackage(new GfxEntityRemovedPackage(received.id));
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(gfxEntityRemovedPackage);
    }

    graphicsEntities.erase(received.id); //temphack
}

void Server::handleMessage(const PlayerConnectedToEntityMessage& received)
{
    std::shared_ptr<BasePackage> playerConnectedToEntityPackage(new PlayerConnectedToEntityPackage(received.playerId, received.entityId));
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(playerConnectedToEntityPackage);
    }
}

void Server::handleMessage(const PlayerFacingBlockMessage& received)
{
    size_t id = received.playerId;
    VoxelCoord vector = received.voxelPosition;
    int x;
    int y;
    int z;

    x = vector.x;
    y = vector.y;
    z = vector.z;

    std::shared_ptr<BasePackage> playerFacingBlockPackage(new PlayerFacingBlockPackage(id, x, y, z));
    mClients.at(id)->enqueuePackage(playerFacingBlockPackage);

}

void Server::handleMessage(const VoxelSetMessage& received)
{
    std::shared_ptr<BasePackage> voxelSetPackage(new VoxelSetPackage(received.voxel , received.type));
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(voxelSetPackage);
    }
}

void Server::setClientListener(std::unique_ptr<ClientConnectionListener> clientListener)
{
    mListener = std::move(clientListener);
}

void Server::acceptClientConnection(std::shared_ptr<ClientConnection> client)
{
    ClientId newClientId = client->getId();

    mClients.emplace(newClientId, client);

    mBus.send<LogMessage>(LogMessage{std::string("Client id ") + std::to_string(newClientId) + std::string(" connected"), mLogName, LogLevel::INFO});

    std::shared_ptr<BasePackage> playerIdPackage(new PlayerIdPackage(newClientId));
    client->enqueuePackage(playerIdPackage);

    //resend current gfx entities. this is a hack right now. in the futuer it probably has to send the whole game state or something, i dunno
    for(size_t id : graphicsEntities)
    {
        std::shared_ptr<BasePackage> gfxEntityAddedPackage(new GfxEntityAddedPackage(id, glm::vec3(0.0f, 0.0f, 0.0f)));
        client->enqueuePackage(gfxEntityAddedPackage);
    }

    //resend chunk created messages
    for(const auto& chunk : mWorlds.getWorldInterface().getChunkMap(0))  //for world 0, thish shouldn't be hard coded
    {
        std::shared_ptr<BasePackage> chunkAddedPackage(new ChunkLoadedPackage(chunk.first, chunk.second.getVoxelTypeData().mRleSegmentIndices, chunk.second.getVoxelTypeData().mRleSegments));
        client->enqueuePackage(chunkAddedPackage);
    }

    mBus.send<PlayerJoinedMessage>(PlayerJoinedMessage{newClientId, 0, glm::vec3(0.0f, 45.0f, 0.0f)}); //position and world could be loaded from file or at spawn
}

void Server::pollNewClients()
{
    std::shared_ptr<ClientConnection> client;

    if(mListener != nullptr)
    {
        while((client = mListener->fetchIncomingConnection()))
        {
            acceptClientConnection(client);
        }
    }
}

void Server::fetchClientData(std::weak_ptr<ClientConnection> client)
{
    std::shared_ptr<BasePackage> package;

    for(auto& client : mClients)
    {
        while(client.second->pollPackage(package))
        {
            if(package->mType == PackageType::REBUILD_SCRIPTS_REQUESTED)
            {
                mBus.send<RebuildScriptsRequestedMessage>(RebuildScriptsRequestedMessage());
            }
            else if(package->mType == PackageType::PLAYER_ACTION)
            {
                PlayerActionPackage* playerActionPackage = (PlayerActionPackage*) package.get();
                mBus.send<PlayerActionMessage>(PlayerActionMessage{std::get<0>(playerActionPackage->getData()), std::get<1>(playerActionPackage->getData())});
            }
            else if(package->mType == PackageType::PLAYER_MOVE_DIRECTION)
            {
                PlayerMoveDirectionPackage* playerMoveDirectionPackage = (PlayerMoveDirectionPackage*) package.get();
                size_t id;
                int8_t forwardsBack;
                int8_t leftRight;
                std::tie(id, forwardsBack, leftRight) = playerMoveDirectionPackage->getData();

                MoveDirection dir(forwardsBack, leftRight);

                mBus.send<PlayerMoveDirectionMessage>(PlayerMoveDirectionMessage{id, dir});
            }
            else if(package->mType == PackageType::PLAYER_MOVE_ACTION)
            {
                PlayerMoveActionPackage* playerMoveActionPackage = (PlayerMoveActionPackage*) package.get();

                mBus.send<PlayerMoveActionMessage>(PlayerMoveActionMessage{std::get<0>(playerMoveActionPackage->getData()), std::get<1>(playerMoveActionPackage->getData())});
            }
            else if(package->mType == PackageType::PLAYER_PITCH_YAW)
            {
                PlayerPitchYawPackage* playerPitchYawPackage = (PlayerPitchYawPackage*) package.get();
                mBus.send<PlayerPitchYawMessage>(PlayerPitchYawMessage{std::get<0>(playerPitchYawPackage->getData()), std::get<1>(playerPitchYawPackage->getData()),std::get<2>( playerPitchYawPackage->getData())});
            }
        }
    }
}

void Server::checkForDisconnectedClients()
{
    std::vector<size_t> clientsToRemove;

    for(auto& client : mClients)
    {
        if(client.second->isDisconnected())
        {
            clientsToRemove.push_back(client.first);
        }
    }

    for(auto client : clientsToRemove)
    {
        mClients.erase(client);
        //send playerdisconnectedmessage
        mBus.send<PlayerDisconnectedMessage>(PlayerDisconnectedMessage{client});
    }
}
