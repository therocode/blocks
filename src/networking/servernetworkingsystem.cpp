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
            mENetServer = std::unique_ptr<ENetServer>(new ENetServer(*mENet, 56556));
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
            mENetServer = std::unique_ptr<ENetServer>(new ENetServer(*mENet, 56556));
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
    mENetServer->update(0);
}

void ServerNetworkingSystem::handleMessage(const IncomingConnectionMessage& received)
{
    uint32_t newId = mNextClientId++;

    mClientToPlayerIds.emplace(received.clientId, newId);
    mPlayerToClientIds.emplace(newId, received.clientId);

    mBus.send(LogMessage{"Client Id " + std::to_string(received.clientId) + " connected and given player Id " + std::to_string(newId), netName, LogLevel::INFO});
    mBus.send(PlayerJoinedMessage{newId, 0, {0.0f, 0.0f, 0.0f}});
}

void ServerNetworkingSystem::handleMessage(const ClientPackageReceived& received)
{
    handleClientPackage(received.clientId, received.package);
}

void ServerNetworkingSystem::handleMessage(const ClientDisconnectedMessage& received)
{
    std::cout << "client id " << received.clientId << " disconnected!\n";

    FEA_ASSERT(mClientToPlayerIds.count(received.clientId) != 0, "Client " << received.clientId << " disconnecting, but that client is not marked as connected!");

    uint32_t playerId = mClientToPlayerIds.at(received.clientId);

    mBus.send(LogMessage{"Client Id " + std::to_string(received.clientId) + ", player Id " + std::to_string(playerId) + " disconnected", netName, LogLevel::INFO});
    mBus.send(PlayerDisconnectedMessage{playerId});

    mClientToPlayerIds.erase(received.clientId);
    mPlayerToClientIds.erase(playerId);
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
        mENetServer->sendToAll(std::move(chunkLoadedPackage));
}

void ServerNetworkingSystem::handleMessage(const ChunkDeletedMessage& received)
{
    std::unique_ptr<BasePackage> chunkDeletedPackage(new ChunkDeletedPackage{received.coordinate});

    if(mLocalClientBus)
        mLocalClientBus->send(received);
    if(mENetServer)
        mENetServer->sendToAll(std::move(chunkDeletedPackage));
}

void ServerNetworkingSystem::handleMessage(const AddGfxEntityMessage& received)
{
    std::unique_ptr<BasePackage> gfxEntityAddedPackage(new GfxEntityAddedPackage(received.id, received.position));

    graphicsEntities.insert(received.id); //temphack

    if(mLocalClientBus)
        mLocalClientBus->send(received);
    if(mENetServer)
        mENetServer->sendToAll(std::move(gfxEntityAddedPackage));
}

void ServerNetworkingSystem::handleMessage(const MoveGfxEntityMessage& received)
{
    std::unique_ptr<BasePackage> gfxEntityMovedPackage(new GfxEntityMovedPackage(received.id, received.position));

    if(mLocalClientBus)
        mLocalClientBus->send(received);
    if(mENetServer)
        mENetServer->sendToAll(std::move(gfxEntityMovedPackage));
}

void ServerNetworkingSystem::handleMessage(const RotateGfxEntityMessage& received)
{
    std::unique_ptr<BasePackage> gfxEntityRotatedPackage(new GfxEntityRotatedPackage(received.id, received.pitch, received.yaw));

    if(mLocalClientBus)
        mLocalClientBus->send(received);
    if(mENetServer)
        mENetServer->sendToAll(std::move(gfxEntityRotatedPackage));
}

void ServerNetworkingSystem::handleMessage(const RemoveGfxEntityMessage& received)
{
    std::unique_ptr<BasePackage> gfxEntityRemovedPackage(new GfxEntityRemovedPackage(received.id));

    graphicsEntities.erase(received.id); //temphack

    if(mLocalClientBus)
        mLocalClientBus->send(received);
    if(mENetServer)
        mENetServer->sendToAll(std::move(gfxEntityRemovedPackage));
}

void ServerNetworkingSystem::handleMessage(const PlayerConnectedToEntityMessage& received)
{
    std::unique_ptr<BasePackage> playerConnectedToEntityPackage(new PlayerConnectedToEntityPackage(received.playerId, received.entityId));

    if(mLocalClientBus)
        mLocalClientBus->send(received);
    if(mENetServer)
        mENetServer->sendToAll(std::move(playerConnectedToEntityPackage));
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
        mENetServer->sendToOne(mPlayerToClientIds.count(id), std::move(playerFacingBlockPackage));
    }
}

void ServerNetworkingSystem::handleMessage(const VoxelSetMessage& received)
{
    std::unique_ptr<BasePackage> voxelSetPackage(new VoxelSetPackage(received.voxel , received.type));

    if(mLocalClientBus)
        mLocalClientBus->send(received);
    if(mENetServer)
        mENetServer->sendToAll(std::move(voxelSetPackage));
}

void ServerNetworkingSystem::handleClientPackage(uint32_t clientId, const std::unique_ptr<BasePackage>& package)
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
