#include "clientnetworkingsystem.hpp"
#include "networkingmessages.hpp"
#include "packages.hpp"
#include "../world/worldmessages.hpp"
#include "../lognames.hpp"
#include "enetclient.hpp"

ClientNetworkingSystem::ClientNetworkingSystem(fea::MessageBus& bus, const NetworkParameters& parameters) :
    mBus(bus),
    mParameters(parameters)
{
    subscribe(mBus, *this);

    if(parameters.mode == NetworkMode::SINGLE_PLAYER)
    {
        mBus.send(LogMessage{"Setting up single player client", netName, LogLevel::INFO});
        mBus.send(LocalConnectionAttemptMessage{&bus});
    }
    else if(parameters.mode == NetworkMode::JOIN)
    {
        mENet = std::unique_ptr<ENet>(new ENet());

        if(mENet->isInitialized())
        {
            mBus.send(LogMessage{"Setting up client networking", netName, LogLevel::INFO});

            mENetClient = std::unique_ptr<ENetClient>(new ENetClient(*mENet));
            mENetClient->setConnectedCallback(std::bind(&ClientNetworkingSystem::connectedToServer, this));
            mENetClient->setDataReceivedCallback(std::bind(&ClientNetworkingSystem::handleServerData, this, std::placeholders::_1));
            mENetClient->setDisconnectedCallback(std::bind(&ClientNetworkingSystem::disconnectedFromServer, this));

            mENetClient->connect(parameters.address, parameters.port, 5000);
        }
        else
        {
            mBus.send(LogMessage{"Could not initialize networking", netName, LogLevel::ERR});
        }
    }
    else if(parameters.mode == NetworkMode::COMBINED)
    {
        mBus.send(LogMessage{"Setting up client networking", netName, LogLevel::INFO});
        mBus.send(LocalConnectionAttemptMessage{&bus});

        mENetClient = std::unique_ptr<ENetClient>(new ENetClient(*mENet));
    }
}

void ClientNetworkingSystem::handleMessage(const FrameMessage& received)
{
    if(mENetClient)
        mENetClient->update(0);
}

void ClientNetworkingSystem::handleMessage(const PlayerActionMessage& received)
{
    PlayerActionPackage package(received.playerId, received.action);
    mENetClient->send(package.getAsBytes(), true, CHANNEL_ENTITY);
}

void ClientNetworkingSystem::handleMessage(const PlayerMoveDirectionMessage& received)
{
    PlayerMoveDirectionPackage package(received.id, received.direction.getForwardBack(), received.direction.getLeftRight());
    mENetClient->send(package.getAsBytes(), true, CHANNEL_ENTITY);
}

void ClientNetworkingSystem::handleMessage(const PlayerMoveActionMessage& received)
{
    PlayerMoveActionPackage package(received.id, received.action);
    mENetClient->send(package.getAsBytes(), true, CHANNEL_ENTITY);
}

void ClientNetworkingSystem::handleMessage(const PlayerPitchYawMessage& received)
{
    PlayerPitchYawPackage package(received.playerId, received.pitch, received.yaw);
    mENetClient->send(package.getAsBytes(), false, CHANNEL_ENTITY);
}

void ClientNetworkingSystem::handleMessage(const RebuildScriptsRequestedMessage& received)
{
    RebuildScriptsRequestedPackage package('0');
    mENetClient->send(package.getAsBytes(), true, CHANNEL_ENTITY);
}

void ClientNetworkingSystem::connectedToServer()
{
    mBus.send(LogMessage{"Successfully connected to server", netName, LogLevel::INFO});
}

void ClientNetworkingSystem::handleServerData(const std::vector<uint8_t>& data)
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
        case PackageType::CHUNK_LOADED:
        {
            package = std::unique_ptr<ChunkLoadedPackage>(new ChunkLoadedPackage());
            package->setFromBytes(data);
            break;
        }
        case PackageType::VOXEL_SET:
        {
            package = std::unique_ptr<VoxelSetPackage>(new VoxelSetPackage());
            package->setFromBytes(data);
            break;
        }
        case PackageType::CHUNK_DELETED:
        {
            package = std::unique_ptr<ChunkDeletedPackage>(new ChunkDeletedPackage());
            package->setFromBytes(data);
            break;
        }
        case PackageType::GFX_ENTITY_ADDED:
        {
            package = std::unique_ptr<GfxEntityAddedPackage>(new GfxEntityAddedPackage());
            package->setFromBytes(data);
            break;
        }
        case PackageType::GFX_ENTITY_MOVED:
        {
            package = std::unique_ptr<GfxEntityMovedPackage>(new GfxEntityMovedPackage());
            package->setFromBytes(data);
            break;
        }
        case PackageType::GFX_ENTITY_ROTATED:
        {
            package = std::unique_ptr<GfxEntityRotatedPackage>(new GfxEntityRotatedPackage());
            package->setFromBytes(data);
            break;
        }
        case PackageType::GFX_ENTITY_REMOVED:
        {
            package = std::unique_ptr<GfxEntityRemovedPackage>(new GfxEntityRemovedPackage());
            package->setFromBytes(data);
            break;
        }
        case PackageType::PLAYER_ID:
        {
            package = std::unique_ptr<PlayerIdPackage>(new PlayerIdPackage());
            package->setFromBytes(data);
            break;
        }
        case PackageType::PLAYER_CONNECTED_TO_ENTITY:
        {
            package = std::unique_ptr<PlayerConnectedToEntityPackage>(new PlayerConnectedToEntityPackage());
            package->setFromBytes(data);
            break;
        }
        case PackageType::PLAYER_FACING_BLOCK:
        {
            package = std::unique_ptr<PlayerFacingBlockPackage>(new PlayerFacingBlockPackage());
            package->setFromBytes(data);
            break;
        }
        default:
        {
            mBus.send(LogMessage{"Unknown package received of type " + std::to_string((int32_t) type), netName, LogLevel::WARN});
            return;
        }
    }

    if(package->mType == PackageType::CHUNK_LOADED)
    {
        ChunkLoadedPackage* chunkPackage = (ChunkLoadedPackage*)package.get();
        ChunkCoord coordinate;

        RleIndexArray rleSegmentIndices;
        RleSegmentArray rleSegments;

        std::tie(coordinate, rleSegmentIndices, rleSegments) = chunkPackage->getData();

        mBus.send(ChunkLoadedMessage{Chunk(coordinate, rleSegmentIndices, rleSegments), 0}); // the 0 here is timestamp. this shall change
    }
    else if(package->mType == PackageType::VOXEL_SET)
    {
        VoxelSetPackage* voxelSetPackage = (VoxelSetPackage*)package.get();

        VoxelCoord voxelCoord;
        VoxelType type;

        std::tie(voxelCoord, type) = voxelSetPackage->getData();

        mBus.send(VoxelSetMessage{voxelCoord, type});
    }
    else if(package->mType == PackageType::CHUNK_DELETED)
    {
        ChunkDeletedPackage* chunkPackage = (ChunkDeletedPackage*)package.get();

        ChunkCoord coordinate;

        std::tie(coordinate) = chunkPackage->getData();

        mBus.send(ClientChunkDeletedMessage{std::get<0>(chunkPackage->getData())});
    }
    else if(package->mType == PackageType::GFX_ENTITY_ADDED)
    {
        GfxEntityAddedPackage* gfxAddedPackage = (GfxEntityAddedPackage*)package.get();
        mBus.send(AddGfxEntityMessage{std::get<0>(gfxAddedPackage->getData()), std::get<1>(gfxAddedPackage->getData())});
    }
    else if(package->mType == PackageType::GFX_ENTITY_MOVED)
    {
        GfxEntityMovedPackage* gfxMovedPackage = (GfxEntityMovedPackage*)package.get();
        mBus.send(MoveGfxEntityMessage{std::get<0>(gfxMovedPackage->getData()), std::get<1>(gfxMovedPackage->getData())});
    }
    else if(package->mType == PackageType::GFX_ENTITY_ROTATED)
    {
        GfxEntityRotatedPackage* gfxEntityRotatedPackage = (GfxEntityRotatedPackage*)package.get();
        mBus.send(RotateGfxEntityMessage{std::get<0>(gfxEntityRotatedPackage->getData()), std::get<1>(gfxEntityRotatedPackage->getData()), std::get<2>(gfxEntityRotatedPackage->getData())});
    }
    else if(package->mType == PackageType::GFX_ENTITY_REMOVED)
    {
        GfxEntityRemovedPackage* gfxRemovedPackage = (GfxEntityRemovedPackage*)package.get();
        mBus.send(RemoveGfxEntityMessage{std::get<0>(gfxRemovedPackage->getData())});
    }
    else if(package->mType == PackageType::PLAYER_ID)
    {
        PlayerIdPackage* playerIdPackage = (PlayerIdPackage*)package.get();
        mBus.send(PlayerIdMessage{std::get<0>(playerIdPackage->getData())});
    }
    else if(package->mType == PackageType::PLAYER_CONNECTED_TO_ENTITY)
    {
        PlayerConnectedToEntityPackage* playerConnectedToEntityPackage = (PlayerConnectedToEntityPackage*)package.get();
        mBus.send(PlayerConnectedToEntityMessage{std::get<0>(playerConnectedToEntityPackage->getData()), std::get<1>(playerConnectedToEntityPackage->getData())});
    }
    else if(package->mType == PackageType::PLAYER_FACING_BLOCK)
    {
        PlayerFacingBlockPackage* playerFacingBlockPackage = (PlayerFacingBlockPackage*)package.get();

        size_t playerId;

        int x;
        int y;
        int z;

        std::tie(playerId, x, y, z) = playerFacingBlockPackage->getData();

        mBus.send(PlayerFacingBlockMessage{playerId, VoxelCoord(x, y, z)});
    }
}

void ClientNetworkingSystem::disconnectedFromServer()
{
    mBus.send(LogMessage{"Disconnected from server", netName, LogLevel::INFO});
}
