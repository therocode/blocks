#include "servernetworkingsystem.hpp"
#include "../world/chunk.hpp"
#include "packages.hpp"
#include "clientconnection.hpp"
#include "localclientconnectionlistener.hpp"
#include "remoteclientconnectionlistener.hpp"
#include "../lognames.hpp"

ServerNetworkingSystem::ServerNetworkingSystem(fea::MessageBus& bus, const NetworkParameters& parameters) :
    mBus(bus),
    mParameters(parameters)
{
    subscribe(mBus, *this);

    if(parameters.mode == NetworkMode::SINGLE_PLAYER)
    {
        LocalClientConnectionListener* listener = new LocalClientConnectionListener();
        mListener = std::unique_ptr<LocalClientConnectionListener>(listener);
    }
    else if(parameters.mode == NetworkMode::DEDICATED)
    {
        RemoteClientConnectionListener* remoteListener = new RemoteClientConnectionListener(mBus);

        if(enet_initialize() < 0)
        {
            mBus.send(LogMessage{"Couldn't initialise enet", "network", LogLevel::ERR});
        }
        else
        {
            remoteListener->startListening(parameters.port);
            mListener = std::unique_ptr<RemoteClientConnectionListener>(remoteListener);
        }
    }
}

void ServerNetworkingSystem::handleMessage(const LocalConnectionAttemptMessage& received)
{
    static_cast<LocalClientConnectionListener*>(mListener.get())->createClientConnection(received.clientToServerBridge);
}

void ServerNetworkingSystem::handleMessage(const FrameMessage& received)
{
    for(auto& client : mClients)
    {
        fetchClientData(client.second);
    }


    for(auto& client : mClients)
    {
        client.second->flush();
    }

    pollNewClients();
}

void ServerNetworkingSystem::handleMessage(const ChunkLoadedMessage& received)
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

void ServerNetworkingSystem::handleMessage(const ChunkDeletedMessage& received)
{
    std::shared_ptr<BasePackage> chunkDeletedPackage(new ChunkDeletedPackage{received.coordinate});
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(chunkDeletedPackage);
    }
}

void ServerNetworkingSystem::handleMessage(const AddGfxEntityMessage& received)
{
    std::shared_ptr<BasePackage> gfxEntityAddedPackage(new GfxEntityAddedPackage(received.id, received.position));
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(gfxEntityAddedPackage);
    }

    graphicsEntities.insert(received.id); //temphack
}

void ServerNetworkingSystem::handleMessage(const MoveGfxEntityMessage& received)
{
    std::shared_ptr<BasePackage> gfxEntityMovedPackage(new GfxEntityMovedPackage(received.id, received.position));
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(gfxEntityMovedPackage);
    }
}

void ServerNetworkingSystem::handleMessage(const RotateGfxEntityMessage& received)
{
    std::shared_ptr<BasePackage> gfxEntityRotatedPackage(new GfxEntityRotatedPackage(received.id, received.pitch, received.yaw));
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(gfxEntityRotatedPackage);
    }
}

void ServerNetworkingSystem::handleMessage(const RemoveGfxEntityMessage& received)
{
    std::shared_ptr<BasePackage> gfxEntityRemovedPackage(new GfxEntityRemovedPackage(received.id));
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(gfxEntityRemovedPackage);
    }

    graphicsEntities.erase(received.id); //temphack
}

void ServerNetworkingSystem::handleMessage(const PlayerConnectedToEntityMessage& received)
{
    std::shared_ptr<BasePackage> playerConnectedToEntityPackage(new PlayerConnectedToEntityPackage(received.playerId, received.entityId));
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(playerConnectedToEntityPackage);
    }
}

void ServerNetworkingSystem::handleMessage(const PlayerFacingBlockMessage& received)
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

void ServerNetworkingSystem::handleMessage(const VoxelSetMessage& received)
{
    std::shared_ptr<BasePackage> voxelSetPackage(new VoxelSetPackage(received.voxel , received.type));
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(voxelSetPackage);
    }
}



void ServerNetworkingSystem::acceptClientConnection(std::shared_ptr<ClientConnection> client)
{
    ClientId newClientId = client->getId();

    mClients.emplace(newClientId, client);

    mBus.send(LogMessage{std::string("Client id ") + std::to_string(newClientId) + std::string(" connected"), netName, LogLevel::INFO});

    std::shared_ptr<BasePackage> playerIdPackage(new PlayerIdPackage(newClientId));
    client->enqueuePackage(playerIdPackage);

    //resend current gfx entities. this is a hack right now. in the futuer it probably has to send the whole game state or something, i dunno
    for(size_t id : graphicsEntities)
    {
        std::shared_ptr<BasePackage> gfxEntityAddedPackage(new GfxEntityAddedPackage(id, glm::vec3(0.0f, 0.0f, 0.0f)));
        client->enqueuePackage(gfxEntityAddedPackage);
    }

    //resend chunk created messages
    //for(const auto& chunk : mGameInterface.getWorldSystem().getWorld(0).getChunkMap())  //for world 0, thish shouldn't be hard coded
    //{
    //    std::shared_ptr<BasePackage> chunkAddedPackage(new ChunkLoadedPackage(chunk.first, chunk.second.getVoxelTypeData().mRleSegmentIndices, chunk.second.getVoxelTypeData().mRleSegments));
    //    client->enqueuePackage(chunkAddedPackage);
    //}

    mBus.send(PlayerJoinedMessage{newClientId, 0, glm::vec3(0.0f, 45.0f, 0.0f)}); //position and world could be loaded from file or at spawn
}

void ServerNetworkingSystem::pollNewClients()
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

void ServerNetworkingSystem::fetchClientData(std::weak_ptr<ClientConnection> client)
{
    std::shared_ptr<BasePackage> package;

    for(auto& client : mClients)
    {
        while(client.second->pollPackage(package))
        {
            if(package->mType == PackageType::REBUILD_SCRIPTS_REQUESTED)
            {
                mBus.send(RebuildScriptsRequestedMessage());
            }
            else if(package->mType == PackageType::PLAYER_ACTION)
            {
                PlayerActionPackage* playerActionPackage = (PlayerActionPackage*) package.get();
                mBus.send(PlayerActionMessage{std::get<0>(playerActionPackage->getData()), std::get<1>(playerActionPackage->getData())});
            }
            else if(package->mType == PackageType::PLAYER_MOVE_DIRECTION)
            {
                PlayerMoveDirectionPackage* playerMoveDirectionPackage = (PlayerMoveDirectionPackage*) package.get();
                size_t id;
                int8_t forwardsBack;
                int8_t leftRight;
                std::tie(id, forwardsBack, leftRight) = playerMoveDirectionPackage->getData();

                MoveDirection dir(forwardsBack, leftRight);

                mBus.send(PlayerMoveDirectionMessage{id, dir});
            }
            else if(package->mType == PackageType::PLAYER_MOVE_ACTION)
            {
                PlayerMoveActionPackage* playerMoveActionPackage = (PlayerMoveActionPackage*) package.get();

                mBus.send(PlayerMoveActionMessage{std::get<0>(playerMoveActionPackage->getData()), std::get<1>(playerMoveActionPackage->getData())});
            }
            else if(package->mType == PackageType::PLAYER_PITCH_YAW)
            {
                PlayerPitchYawPackage* playerPitchYawPackage = (PlayerPitchYawPackage*) package.get();
                mBus.send(PlayerPitchYawMessage{std::get<0>(playerPitchYawPackage->getData()), std::get<1>(playerPitchYawPackage->getData()),std::get<2>( playerPitchYawPackage->getData())});
            }
        }
    }
}

//void ServerNetworkingSystemcheckForDisconnectedClients()
//{
//    std::vector<size_t> clientsToRemove;
//
//    for(auto& client : mClients)
//    {
//        if(client.second->isDisconnected())
//        {
//            clientsToRemove.push_back(client.first);
//        }
//    }
//
//    for(auto client : clientsToRemove)
//    {
//        mClients.erase(client);
//        //send playerdisconnectedmessage
//        mBus.send(PlayerDisconnectedMessage{client});
//    }
//}
