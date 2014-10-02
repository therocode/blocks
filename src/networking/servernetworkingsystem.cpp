#include "servernetworkingsystem.hpp"
#include "../world/chunk.hpp"
#include "packages.hpp"
#include "../lognames.hpp"
#include "../script/scriptmessages.hpp"
#include "../input/inputmessages.hpp"

ServerNetworkingSystem::ServerNetworkingSystem(fea::MessageBus& bus, const NetworkParameters& parameters) :
    mBus(bus),
    mParameters(parameters),
    mNextClientId(0),
    mLocalPlayerId(0),
    mLocalClientBus(nullptr)
{

    subscribe(mBus, *this);

    if(parameters.mode == NetworkMode::SINGLE_PLAYER)
    {
        mBus.send(LogMessage{"Setting up single player networking", netName, LogLevel::INFO});
    }
    else if(parameters.mode == NetworkMode::DEDICATED)
    {
        mENet = std::unique_ptr<ENet>(new ENet());

        if(mENet->isInitialized())
        {
            mBus.send(LogMessage{"Setting up dedicated server networking", netName, LogLevel::INFO});

            mENetServer = std::unique_ptr<ENetServer>(new ENetServer(*mENet, parameters.port));
            mENetServer->setConnectedCallback(std::bind(&ServerNetworkingSystem::acceptRemoteClient, this, std::placeholders::_1));
            mENetServer->setDataReceivedCallback(std::bind(&ServerNetworkingSystem::handleClientData, this, std::placeholders::_1, std::placeholders::_2));
            mENetServer->setDisconnectedCallback(std::bind(&ServerNetworkingSystem::disconnectRemoteClient, this, std::placeholders::_1));

            if(mENetServer->isListening())
                mBus.send(LogMessage{"Now listening on port " + std::to_string(parameters.port), netName, LogLevel::INFO});
            else
                mBus.send(LogMessage{"Could not bind to port " + std::to_string(parameters.port), netName, LogLevel::ERR});
        }
        else
        {
            mBus.send(LogMessage{"Could not initialize networking", netName, LogLevel::ERR});
        }
    }
    else if(parameters.mode == NetworkMode::COMBINED)
    {
        mENet = std::unique_ptr<ENet>(new ENet());

        if(mENet->isInitialized())
        {
            mBus.send(LogMessage{"Setting up networking", netName, LogLevel::INFO});

            mENetServer = std::unique_ptr<ENetServer>(new ENetServer(*mENet, parameters.port));
            mENetServer = std::unique_ptr<ENetServer>(new ENetServer(*mENet, parameters.port));
            mENetServer->setConnectedCallback(std::bind(&ServerNetworkingSystem::acceptRemoteClient, this, std::placeholders::_1));
            mENetServer->setDataReceivedCallback(std::bind(&ServerNetworkingSystem::handleClientData, this, std::placeholders::_1, std::placeholders::_2));
            mENetServer->setDisconnectedCallback(std::bind(&ServerNetworkingSystem::disconnectRemoteClient, this, std::placeholders::_1));

            if(mENetServer->isListening())
                mBus.send(LogMessage{"Now listening on port " + std::to_string(parameters.port), netName, LogLevel::INFO});
            else
                mBus.send(LogMessage{"Could not bind to port " + std::to_string(parameters.port), netName, LogLevel::ERR});
        }
        else
        {
            mBus.send(LogMessage{"Could not initialize networking", netName, LogLevel::ERR});
        }
    }
}

void ServerNetworkingSystem::handleMessage(const LocalConnectionAttemptMessage& received)
{
    uint32_t newId = mNextClientId++;

    mLocalPlayerId = newId;
    mLocalClientBus = received.clientBus;

    mBus.send(LogMessage{"Client connected locally and given player Id " + std::to_string(newId), netName, LogLevel::INFO});
    mBus.send(PlayerJoinedMessage{newId, 0, {0.0f, 0.0f, 0.0f}});

    mBus.send(LocalConnectionEstablishedMessage{&mBus});
}

void ServerNetworkingSystem::handleMessage(const FrameMessage& received)
{
    if(mENetServer)
        mENetServer->update(0);
}

void ServerNetworkingSystem::handleMessage(const ChunkLoadedMessage& received)
{
    const ChunkCoord& coordinate = received.chunk.getLocation();
    const Chunk& chunk = received.chunk;

    VoxelTypeData typeData = chunk.getVoxelTypeData();

    std::unique_ptr<BasePackage> chunkLoadedPackage(new ChunkLoadedPackage(coordinate, typeData.mRleSegmentIndices, typeData.mRleSegments));

    if(mLocalClientBus)
        mLocalClientBus->send(received);
    if(mENetServer)
        mENetServer->sendToAll(std::move(chunkLoadedPackage->getAsBytes()), true, CHANNEL_CHUNKS);
}

void ServerNetworkingSystem::handleMessage(const ChunkDeletedMessage& received)
{
    std::unique_ptr<BasePackage> chunkDeletedPackage(new ChunkDeletedPackage{received.coordinate});

    if(mLocalClientBus)
        mLocalClientBus->send(received);
    if(mENetServer)
        mENetServer->sendToAll(std::move(chunkDeletedPackage->getAsBytes()), true, CHANNEL_CHUNKS);
}

void ServerNetworkingSystem::handleMessage(const AddGfxEntityMessage& received)
{
    std::unique_ptr<BasePackage> gfxEntityAddedPackage(new GfxEntityAddedPackage(received.id, received.position));

    graphicsEntities.insert(received.id); //temphack

    if(mLocalClientBus)
        mLocalClientBus->send(received);
    if(mENetServer)
        mENetServer->sendToAll(std::move(gfxEntityAddedPackage->getAsBytes()), true, CHANNEL_ENTITY);
}

void ServerNetworkingSystem::handleMessage(const MoveGfxEntityMessage& received)
{
    std::unique_ptr<BasePackage> gfxEntityMovedPackage(new GfxEntityMovedPackage(received.id, received.position));

    if(mLocalClientBus)
        mLocalClientBus->send(received);
    if(mENetServer)
        mENetServer->sendToAll(std::move(gfxEntityMovedPackage->getAsBytes()), false, CHANNEL_ENTITY);
}

void ServerNetworkingSystem::handleMessage(const RotateGfxEntityMessage& received)
{
    std::unique_ptr<BasePackage> gfxEntityRotatedPackage(new GfxEntityRotatedPackage(received.id, received.pitch, received.yaw));

    if(mLocalClientBus)
        mLocalClientBus->send(received);
    if(mENetServer)
        mENetServer->sendToAll(std::move(gfxEntityRotatedPackage->getAsBytes()), false, CHANNEL_ENTITY);
}

void ServerNetworkingSystem::handleMessage(const RemoveGfxEntityMessage& received)
{
    std::unique_ptr<BasePackage> gfxEntityRemovedPackage(new GfxEntityRemovedPackage(received.id));

    graphicsEntities.erase(received.id); //temphack

    if(mLocalClientBus)
        mLocalClientBus->send(received);
    if(mENetServer)
        mENetServer->sendToAll(std::move(gfxEntityRemovedPackage->getAsBytes()), true, CHANNEL_ENTITY);
}

void ServerNetworkingSystem::handleMessage(const PlayerConnectedToEntityMessage& received)
{
    std::unique_ptr<BasePackage> playerConnectedToEntityPackage(new PlayerConnectedToEntityPackage(received.playerId, received.entityId));

    if(mLocalClientBus)
        mLocalClientBus->send(received);
    if(mENetServer)
        mENetServer->sendToAll(std::move(playerConnectedToEntityPackage->getAsBytes()), true, CHANNEL_ENTITY);
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

    std::unique_ptr<BasePackage> playerFacingBlockPackage(new PlayerFacingBlockPackage(id, x, y, z));

    if(mLocalClientBus)
    {
        if(mLocalPlayerId == id)
            mLocalClientBus->send(received);
    }
    if(mENetServer)
    {
        FEA_ASSERT(mPlayerToClientIds.count(id) != 0, "Trying to send packet to player Id " + std::to_string(id) + " but that player has no client attached to it");
        mENetServer->sendToOne(mPlayerToClientIds.count(id), std::move(playerFacingBlockPackage->getAsBytes()), false, CHANNEL_ENTITY);
    }
}

void ServerNetworkingSystem::handleMessage(const VoxelSetMessage& received)
{
    std::unique_ptr<BasePackage> voxelSetPackage(new VoxelSetPackage(received.voxel , received.type));

    if(mLocalClientBus)
        mLocalClientBus->send(received);
    if(mENetServer)
        mENetServer->sendToAll(std::move(voxelSetPackage->getAsBytes()), true, CHANNEL_CHUNKS);
}

void ServerNetworkingSystem::acceptRemoteClient(uint32_t id)
{
    uint32_t newId = mNextClientId++;

    mClientToPlayerIds.emplace(id, newId);
    mPlayerToClientIds.emplace(newId, id);

    mBus.send(LogMessage{"Client Id " + std::to_string(id) + " connected and given player Id " + std::to_string(newId), netName, LogLevel::INFO});
    mBus.send(PlayerJoinedMessage{newId, 0, {0.0f, 0.0f, 0.0f}});
}

void ServerNetworkingSystem::handleClientData(uint32_t clientId, const std::vector<uint8_t>& data)
{
    PackageType type;

    uint8_t* typePointer = (uint8_t*)&type;
    for(uint32_t i = 0; i < sizeof(PackageType); i++)
    {   
        *typePointer = data[i];
        typePointer++;
    }   
    
    std::unique_ptr<BasePackage> package;

    switch(type)
    {   
        case PackageType::REBUILD_SCRIPTS_REQUESTED:
            {   
                package = std::unique_ptr<RebuildScriptsRequestedPackage>();
                package->setFromBytes(data);
                break;
            }   
        case PackageType::PLAYER_ACTION:
            {
                package = std::unique_ptr<PlayerActionPackage>();
                package->setFromBytes(data);
                break;
            }
        case PackageType::PLAYER_MOVE_DIRECTION:
            {
                package = std::unique_ptr<PlayerMoveDirectionPackage>();
                package->setFromBytes(data);
                break;
            }
        case PackageType::PLAYER_MOVE_ACTION:
            {
                package = std::unique_ptr<PlayerMoveActionPackage>();
                package->setFromBytes(data);
                break;
            }
        case PackageType::PLAYER_PITCH_YAW:
            {
                package = std::unique_ptr<PlayerPitchYawPackage>();
                package->setFromBytes(data);
                break;
            }
        default:
            {
                mBus.send(LogMessage{"Unknown package received of type " + std::to_string((int32_t) type), netName, LogLevel::WARN});
                return;
            }
    }

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

void ServerNetworkingSystem::disconnectRemoteClient(uint32_t id)
{
    std::cout << "client id " << id << " disconnected!\n";

    FEA_ASSERT(mClientToPlayerIds.count(id) != 0, "Client " << id << " disconnecting, but that client is not marked as connected!");

    uint32_t playerId = mClientToPlayerIds.at(id);

    mBus.send(LogMessage{"Client Id " + std::to_string(id) + ", player Id " + std::to_string(playerId) + " disconnected", netName, LogLevel::INFO});
    mBus.send(PlayerDisconnectedMessage{playerId});

    mClientToPlayerIds.erase(id);
    mPlayerToClientIds.erase(playerId);
}
