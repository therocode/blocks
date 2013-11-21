#include "client.h"
#include <iostream>
#if 0
#include <featherkit/util/window/sfml/sfmlwindowbackend.h>
#include <featherkit/util/input/sfml/sfmlinputbackend.h>
#else
#include <featherkit/util/window/sdl2/sdl2windowbackend.h>
#include <featherkit/util/input/sdl2/sdl2inputbackend.h>
#endif

#include <typeinfo>
#include "../networking/packages.h"
#include "../application/applicationmessages.h"


Client::Client() :
    mLogger(mBus, LogLevel::VERB),
	mWindow(new fea::util::SDL2WindowBackend()),
	mRenderer(mBus),
	mInputAdaptor(mBus),
	mQuit(false),
	mLogName("client"),
	mBridge(nullptr)
{
	mBus.addMessageSubscriber<PlayerActionMessage>(*this);
	mBus.addMessageSubscriber<PlayerMoveDirectionMessage>(*this);
	mBus.addMessageSubscriber<PlayerMoveActionMessage>(*this);
	mBus.addMessageSubscriber<PlayerPitchYawMessage>(*this);
	mBus.addMessageSubscriber<RebuildScriptsRequestedMessage>(*this);
    mBus.addMessageSubscriber<WindowFocusLostMessage>(*this);
    mBus.addMessageSubscriber<WindowInputMessage>(*this);
}

Client::~Client()
{
	mBus.removeMessageSubscriber<PlayerActionMessage>(*this);
	mBus.removeMessageSubscriber<PlayerMoveDirectionMessage>(*this);
	mBus.removeMessageSubscriber<PlayerMoveActionMessage>(*this);
	mBus.removeMessageSubscriber<PlayerPitchYawMessage>(*this);
	mBus.removeMessageSubscriber<RebuildScriptsRequestedMessage>(*this);
    mBus.removeMessageSubscriber<WindowFocusLostMessage>(*this);
    mBus.removeMessageSubscriber<WindowInputMessage>(*this);
}

bool Client::loadTexture(const std::string& path, uint32_t width, uint32_t height, std::vector<unsigned char>& result)
{

	//decode
	unsigned error = lodepng::decode(result, width, height, path);

	//if there's an error, display it
	if(error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

	return true;
	//the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
}
void Client::setup()
{
	mWindow.create(fea::VideoMode(800, 600, 32), "Blocky", fea::Style::Default, fea::ContextSettings(32));
    mLockedMouse = true;
	mWindow.lockCursor(true);
	// mWindow.setVSyncEnabled(false);
	mRenderer.setup();
	//	mWindow.setFramerateLimit(30);

	mBus.sendMessage<WindowResizeMessage>(WindowResizeMessage(800, 600));

	std::vector<unsigned char> icon;
	loadTexture("data/textures/icon16x16.png", 16, 16, icon);
	mWindow.setIcon(16, 16, icon.data());

	//if there's an error, display it
}

void Client::handleInput()
{
	if(mBridge)
	{
		fetchServerData();
	}

	mInputAdaptor.update();

	if(mBridge)
	{
		mBridge->flush();
	}
}

void Client::render()
{
	mRenderer.cameraUpdate();
	mRenderer.render();
	mWindow.swapBuffers();
}

void Client::destroy()
{
	mWindow.close();
	mBus.sendMessage<LogMessage>(LogMessage("client destroyed", mLogName, LogLevel::INFO));
}

void Client::handleMessage(const PlayerActionMessage& received)
{
	size_t playerId;
	InputAction action;
	std::tie(playerId, action) = received.data;

	if(action == InputAction::QUIT)
	{
		mQuit = true;
	}
	else
	{
        if(!mLockedMouse)return;
		mBridge->enqueuePackage(std::shared_ptr<BasePackage>(new PlayerActionPackage(playerId, action)));
	}
}

void Client::handleMessage(const PlayerMoveDirectionMessage& received)
{
    if(!mLockedMouse)return;
    size_t id;
    MoveDirection dir;

    std::tie(id, dir) = received.data;

    mBridge->enqueuePackage(std::shared_ptr<BasePackage>(new PlayerMoveDirectionPackage(id, dir.getForwardBack(), dir.getLeftRight())));
}

void Client::handleMessage(const PlayerMoveActionMessage& received)
{
    if(!mLockedMouse)return;
    mBridge->enqueuePackage(std::shared_ptr<BasePackage>(new PlayerMoveActionPackage(received.data)));
}

void Client::handleMessage(const PlayerPitchYawMessage& received)
{
    if(!mLockedMouse)return;
	size_t playerId;
	float pitch;
	float yaw;

	std::tie(playerId, pitch, yaw) = received.data;
	mBridge->enqueuePackage(std::shared_ptr<BasePackage>(new PlayerPitchYawPackage(received.data)));
}

void Client::handleMessage(const RebuildScriptsRequestedMessage& received)
{
	mBridge->enqueuePackage(std::shared_ptr<BasePackage>(new RebuildScriptsRequestedPackage('0')));
}

void Client::handleMessage(const WindowFocusLostMessage& received){
    mWindow.lockCursor(false);
    mLockedMouse = false;
}

void Client::handleMessage(const WindowInputMessage& received){
    if(!mLockedMouse){
        mLockedMouse = true;
        mWindow.lockCursor(true);
    }
}

bool Client::requestedQuit()
{
	return mQuit;
}

void Client::setServerBridge(std::unique_ptr<ServerClientBridge> bridge)
{
	mBridge = std::move(bridge);
	mBus.sendMessage<LogMessage>(LogMessage("client connected to server", mLogName, LogLevel::INFO));
}

fea::MessageBus& Client::getBus()
{
    return mBus;
}

void Client::fetchServerData()
{
	std::shared_ptr<BasePackage> package;
	while(mBridge->pollPackage(package))
	{
		if(package->mType == PackageType::CHUNK_LOADED)
		{
			ChunkLoadedPackage* chunkPackage = (ChunkLoadedPackage*)package.get();
			ChunkCoordinate coordinate;

            RleIndexArray rleSegmentIndices;
            RleSegmentArray rleSegments;

			std::tie(coordinate, rleSegmentIndices, rleSegments) = chunkPackage->getData();

            mLocalChunks[coordinate] = Chunk(coordinate, rleSegmentIndices, rleSegments);

            updateChunk(coordinate);

            if(mLocalChunks.find(ChunkCoordinate(coordinate.x + 1, coordinate.y, coordinate.z)) != mLocalChunks.end())
            {
                updateChunk(ChunkCoordinate(coordinate.x + 1, coordinate.y, coordinate.z));
            }
            if(mLocalChunks.find(ChunkCoordinate(coordinate.x - 1, coordinate.y, coordinate.z)) != mLocalChunks.end())
            {
                updateChunk(ChunkCoordinate(coordinate.x - 1, coordinate.y, coordinate.z));
            }
            if(mLocalChunks.find(ChunkCoordinate(coordinate.x, coordinate.y + 1, coordinate.z)) != mLocalChunks.end())
            {
                updateChunk(ChunkCoordinate(coordinate.x, coordinate.y + 1, coordinate.z));
            }
            if(mLocalChunks.find(ChunkCoordinate(coordinate.x, coordinate.y - 1, coordinate.z)) != mLocalChunks.end())
            {
                updateChunk(ChunkCoordinate(coordinate.x, coordinate.y - 1, coordinate.z));
            }
            if(mLocalChunks.find(ChunkCoordinate(coordinate.x, coordinate.y, coordinate.z + 1)) != mLocalChunks.end())
            {
                updateChunk(ChunkCoordinate(coordinate.x, coordinate.y, coordinate.z + 1));
            }
            if(mLocalChunks.find(ChunkCoordinate(coordinate.x, coordinate.y, coordinate.z - 1)) != mLocalChunks.end())
            {
                updateChunk(ChunkCoordinate(coordinate.x, coordinate.y, coordinate.z - 1));
            }
		}
		else if(package->mType == PackageType::VOXEL_SET)
		{
			VoxelSetPackage* voxelSetPackage = (VoxelSetPackage*)package.get();

            ChunkCoordinate chunkCoordinate;
            VoxelCoordinate voxelCoordinate;
            VoxelType type;

			std::tie(chunkCoordinate, voxelCoordinate, type) = voxelSetPackage->getData();

            auto chunk = mLocalChunks.find(chunkCoordinate);

            if(chunk != mLocalChunks.end())
            {
                chunk->second.setVoxelType(voxelCoordinate, type);
                updateChunk(chunkCoordinate);
            }

            ChunkCoordinate leftNeighbour = ChunkCoordinate(chunkCoordinate.x - 1, chunkCoordinate.y, chunkCoordinate.z);
            if(voxelCoordinate.x == 0 && mLocalChunks.find(leftNeighbour) != mLocalChunks.end())
            {
                updateChunk(leftNeighbour);
            }

            ChunkCoordinate rightNeighbour = ChunkCoordinate(chunkCoordinate.x + 1, chunkCoordinate.y, chunkCoordinate.z);
            if(voxelCoordinate.x == chunkWidth - 1 && mLocalChunks.find(rightNeighbour) != mLocalChunks.end())
            {
                updateChunk(rightNeighbour);
            }

            ChunkCoordinate topNeighbour = ChunkCoordinate(chunkCoordinate.x, chunkCoordinate.y + 1, chunkCoordinate.z);
            if(voxelCoordinate.y == chunkWidth - 1 && mLocalChunks.find(topNeighbour) != mLocalChunks.end())
            {
                updateChunk(topNeighbour);
            }

            ChunkCoordinate bottomNeighbour = ChunkCoordinate(chunkCoordinate.x, chunkCoordinate.y - 1, chunkCoordinate.z);
            if(voxelCoordinate.y == 0 && mLocalChunks.find(bottomNeighbour) != mLocalChunks.end())
            {
                updateChunk(bottomNeighbour);
            }

            ChunkCoordinate frontNeighbour = ChunkCoordinate(chunkCoordinate.x, chunkCoordinate.y, chunkCoordinate.z + 1);
            if(voxelCoordinate.z == chunkWidth - 1 && mLocalChunks.find(frontNeighbour) != mLocalChunks.end())
            {
                updateChunk(frontNeighbour);
            }

            ChunkCoordinate backNeighbour = ChunkCoordinate(chunkCoordinate.x, chunkCoordinate.y, chunkCoordinate.z - 1);
            if(voxelCoordinate.z == 0 && mLocalChunks.find(backNeighbour) != mLocalChunks.end())
            {
                updateChunk(backNeighbour);
            }
        }
		else if(package->mType == PackageType::CHUNK_DELETED)
		{
			ChunkDeletedPackage* chunkPackage = (ChunkDeletedPackage*)package.get();

            ChunkCoordinate coordinate;

            std::tie(coordinate) = chunkPackage->getData();

			mBus.sendMessage<ChunkDeletedMessage>(ChunkDeletedMessage(chunkPackage->getData()));

            mLocalChunks.erase(coordinate);
		}
		else if(package->mType == PackageType::GFX_ENTITY_ADDED)
		{
			GfxEntityAddedPackage* gfxAddedPackage = (GfxEntityAddedPackage*)package.get();
			mBus.sendMessage<AddGfxEntityMessage>(AddGfxEntityMessage(gfxAddedPackage->getData()));
		}
		else if(package->mType == PackageType::GFX_ENTITY_MOVED)
		{
			GfxEntityMovedPackage* gfxMovedPackage = (GfxEntityMovedPackage*)package.get();
			mBus.sendMessage<MoveGfxEntityMessage>(MoveGfxEntityMessage(gfxMovedPackage->getData()));
		}
		else if(package->mType == PackageType::GFX_ENTITY_ROTATED)
		{
			GfxEntityRotatedPackage* gfxEntityRotatedPackage = (GfxEntityRotatedPackage*)package.get();
			mBus.sendMessage<RotateGfxEntityMessage>(RotateGfxEntityMessage(gfxEntityRotatedPackage->getData()));
		}
		else if(package->mType == PackageType::GFX_ENTITY_REMOVED)
		{
			GfxEntityRemovedPackage* gfxRemovedPackage = (GfxEntityRemovedPackage*)package.get();
			mBus.sendMessage<RemoveGfxEntityMessage>(RemoveGfxEntityMessage(gfxRemovedPackage->getData()));
		}
		else if(package->mType == PackageType::PLAYER_ID)
		{
			PlayerIdPackage* playerIdPackage = (PlayerIdPackage*)package.get();
			mBus.sendMessage<PlayerIdMessage>(PlayerIdMessage(playerIdPackage->getData()));
		}
		else if(package->mType == PackageType::PLAYER_CONNECTED_TO_ENTITY)
		{
			PlayerConnectedToEntityPackage* playerConnectedToEntityPackage = (PlayerConnectedToEntityPackage*)package.get();
			mBus.sendMessage<PlayerConnectedToEntityMessage>(PlayerConnectedToEntityMessage(playerConnectedToEntityPackage->getData()));
		}
		else if(package->mType == PackageType::PLAYER_FACING_BLOCK)
		{
			PlayerFacingBlockPackage* playerFacingBlockPackage = (PlayerFacingBlockPackage*)package.get();

            size_t playerId;

            int x;
            int y;
            int z;

            std::tie(playerId, x, y, z) = playerFacingBlockPackage->getData();

			mBus.sendMessage<PlayerFacingBlockMessage>(PlayerFacingBlockMessage(playerId, VoxelWorldCoordinate(x, y, z)));
		}
	}
}

void Client::updateChunk(const ChunkCoordinate& coordinate)
{
    Chunk* mainChunk = &mLocalChunks.at(coordinate);
    Chunk* topChunk = nullptr;
    Chunk* bottomChunk = nullptr;
    Chunk* frontChunk = nullptr;
    Chunk* backChunk = nullptr;
    Chunk* leftChunk = nullptr;
    Chunk* rightChunk = nullptr;

    auto top = mLocalChunks.find(ChunkCoordinate(coordinate.x, coordinate.y + 1, coordinate.z));
    auto bottom = mLocalChunks.find(ChunkCoordinate(coordinate.x, coordinate.y - 1, coordinate.z));
    auto front = mLocalChunks.find(ChunkCoordinate(coordinate.x, coordinate.y, coordinate.z + 1));
    auto back = mLocalChunks.find(ChunkCoordinate(coordinate.x, coordinate.y, coordinate.z - 1));
    auto left = mLocalChunks.find(ChunkCoordinate(coordinate.x - 1, coordinate.y, coordinate.z));
    auto right = mLocalChunks.find(ChunkCoordinate(coordinate.x + 1, coordinate.y, coordinate.z));

    if(top != mLocalChunks.end())
        topChunk = &top->second;
    if(bottom != mLocalChunks.end())
        bottomChunk = &bottom->second;
    if(front != mLocalChunks.end())
        frontChunk = &front->second;
    if(back != mLocalChunks.end())
        backChunk = &back->second;
    if(left != mLocalChunks.end())
        leftChunk = &left->second;
    if(right != mLocalChunks.end())
        rightChunk = &right->second;

    mBus.sendMessage<UpdateChunkVboMessage>(UpdateChunkVboMessage(mainChunk, topChunk, bottomChunk, frontChunk, backChunk, leftChunk, rightChunk));
}
