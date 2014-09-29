#include "clientnetworkingsystem.hpp"
#include "localserverclientbridge.hpp"
#include "networkingmessages.hpp"
#include "packages.hpp"

ClientNetworkingSystem::ClientNetworkingSystem(fea::MessageBus& bus, const NetworkParameters& parameters) :
    mBus(bus),
    mParameters(parameters),
	mBridge(nullptr)
{
    subscribe(mBus, *this);
    LocalServerClientBridge* clientToServer = new LocalServerClientBridge();
    mBridge = std::unique_ptr<LocalServerClientBridge>(clientToServer);

    mBus.send(LocalConnectionAttemptMessage{clientToServer});
}

void ClientNetworkingSystem::handleMessage(const FrameMessage& received)
{
	if(mBridge)
	{
		fetchServerData();
	}

	if(mBridge)
	{
		mBridge->flush();
	}
}

void ClientNetworkingSystem::handleMessage(const PlayerActionMessage& received)
{
    mBridge->enqueuePackage(std::shared_ptr<BasePackage>(new PlayerActionPackage(received.playerId, received.action)));
}

void ClientNetworkingSystem::handleMessage(const PlayerMoveDirectionMessage& received)
{
    mBridge->enqueuePackage(std::shared_ptr<BasePackage>(new PlayerMoveDirectionPackage(received.id, received.direction.getForwardBack(), received.direction.getLeftRight())));
}

void ClientNetworkingSystem::handleMessage(const PlayerMoveActionMessage& received)
{
    mBridge->enqueuePackage(std::shared_ptr<BasePackage>(new PlayerMoveActionPackage(received.id, received.action)));
}

void ClientNetworkingSystem::handleMessage(const PlayerPitchYawMessage& received)
{
	mBridge->enqueuePackage(std::shared_ptr<BasePackage>(new PlayerPitchYawPackage(received.playerId, received.pitch, received.yaw)));
}

void ClientNetworkingSystem::handleMessage(const RebuildScriptsRequestedMessage& received)
{
	mBridge->enqueuePackage(std::shared_ptr<BasePackage>(new RebuildScriptsRequestedPackage('0')));
}

void ClientNetworkingSystem::fetchServerData()
{
	std::shared_ptr<BasePackage> package;
	while(mBridge->pollPackage(package))
	{
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
}
