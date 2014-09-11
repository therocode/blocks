#include "client.hpp"
#include <iostream>
#include <fea/ui/sdl2windowbackend.hpp>
#include <fea/ui/sdl2inputbackend.hpp>
#include "../networking/packages.hpp"
#include "../application/applicationmessages.hpp"


Client::Client() :
    mLogger(mBus, LogLevel::VERB),
	mWindow(new fea::SDL2WindowBackend()),
	mRenderer(mBus),
	mInputAdaptor(mBus),
	mQuit(false),
	mLogName("client"),
	mBridge(nullptr)
{
	mBus.addSubscriber<PlayerActionMessage>(*this);
	mBus.addSubscriber<PlayerMoveDirectionMessage>(*this);
	mBus.addSubscriber<PlayerMoveActionMessage>(*this);
	mBus.addSubscriber<PlayerPitchYawMessage>(*this);
	mBus.addSubscriber<RebuildScriptsRequestedMessage>(*this);
    mBus.addSubscriber<WindowFocusLostMessage>(*this);
    mBus.addSubscriber<WindowInputMessage>(*this);
}

Client::~Client()
{
	mBus.removeSubscriber<PlayerActionMessage>(*this);
	mBus.removeSubscriber<PlayerMoveDirectionMessage>(*this);
	mBus.removeSubscriber<PlayerMoveActionMessage>(*this);
	mBus.removeSubscriber<PlayerPitchYawMessage>(*this);
	mBus.removeSubscriber<RebuildScriptsRequestedMessage>(*this);
    mBus.removeSubscriber<WindowFocusLostMessage>(*this);
    mBus.removeSubscriber<WindowInputMessage>(*this);
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
	mWindow.setVSyncEnabled(false);
	mRenderer.setup();
	//mWindow.setFramerateLimit(30);

	mBus.send<WindowResizeMessage>(WindowResizeMessage{800, 600});

	std::vector<unsigned char> icon;
	loadTexture("data/textures/icon16x16.png", 16, 16, icon);
	mWindow.setIcon(16, 16, icon.data());
    mFPSCounter.setMaxFPS(0);
    mFPSCounter.setSampleTime(0.5f);

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
    mFPSCounter.frameBegin();
	mRenderer.render();
	mWindow.swapBuffers();
    mFPSCounter.frameEnd();
    if(mFrame++ % 60 == 0)
    mWindow.setTitle("Blocks | FPS:" + std::to_string((int)mFPSCounter.getAverageFPS()));
}

void Client::destroy()
{
	mWindow.close();
	mBus.send<LogMessage>(LogMessage{"client destroyed", mLogName, LogLevel::INFO});
}

void Client::handleMessage(const PlayerActionMessage& received)
{
	if(received.action == InputAction::QUIT)
	{
		mQuit = true;
	}
	else
	{
        if(!mLockedMouse)return;
		mBridge->enqueuePackage(std::shared_ptr<BasePackage>(new PlayerActionPackage(received.playerId, received.action)));
	}
}

void Client::handleMessage(const PlayerMoveDirectionMessage& received)
{
    if(!mLockedMouse)return;

    mBridge->enqueuePackage(std::shared_ptr<BasePackage>(new PlayerMoveDirectionPackage(received.id, received.direction.getForwardBack(), received.direction.getLeftRight())));
}

void Client::handleMessage(const PlayerMoveActionMessage& received)
{
    if(!mLockedMouse)return;
    mBridge->enqueuePackage(std::shared_ptr<BasePackage>(new PlayerMoveActionPackage(received.id, received.action)));
}

void Client::handleMessage(const PlayerPitchYawMessage& received)
{
    if(!mLockedMouse)return;

	mBridge->enqueuePackage(std::shared_ptr<BasePackage>(new PlayerPitchYawPackage(received.playerId, received.pitch, received.yaw)));
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
	mBus.send<LogMessage>(LogMessage{"client connected to server", mLogName, LogLevel::INFO});
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
			ChunkCoord coordinate;

            RleIndexArray rleSegmentIndices;
            RleSegmentArray rleSegments;

			std::tie(coordinate, rleSegmentIndices, rleSegments) = chunkPackage->getData();

            mLocalChunks[coordinate] = Chunk(coordinate, rleSegmentIndices, rleSegments);

            updateChunk(coordinate);

            if(mLocalChunks.find(ChunkCoord(coordinate.x + 1, coordinate.y, coordinate.z)) != mLocalChunks.end())
            {
                updateChunk(ChunkCoord(coordinate.x + 1, coordinate.y, coordinate.z));
            }
            if(mLocalChunks.find(ChunkCoord(coordinate.x - 1, coordinate.y, coordinate.z)) != mLocalChunks.end())
            {
                updateChunk(ChunkCoord(coordinate.x - 1, coordinate.y, coordinate.z));
            }
            if(mLocalChunks.find(ChunkCoord(coordinate.x, coordinate.y + 1, coordinate.z)) != mLocalChunks.end())
            {
                updateChunk(ChunkCoord(coordinate.x, coordinate.y + 1, coordinate.z));
            }
            if(mLocalChunks.find(ChunkCoord(coordinate.x, coordinate.y - 1, coordinate.z)) != mLocalChunks.end())
            {
                updateChunk(ChunkCoord(coordinate.x, coordinate.y - 1, coordinate.z));
            }
            if(mLocalChunks.find(ChunkCoord(coordinate.x, coordinate.y, coordinate.z + 1)) != mLocalChunks.end())
            {
                updateChunk(ChunkCoord(coordinate.x, coordinate.y, coordinate.z + 1));
            }
            if(mLocalChunks.find(ChunkCoord(coordinate.x, coordinate.y, coordinate.z - 1)) != mLocalChunks.end())
            {
                updateChunk(ChunkCoord(coordinate.x, coordinate.y, coordinate.z - 1));
            }
		}
		else if(package->mType == PackageType::VOXEL_SET)
		{
			VoxelSetPackage* voxelSetPackage = (VoxelSetPackage*)package.get();

            VoxelCoord voxelCoord;
            VoxelType type;

			std::tie(voxelCoord, type) = voxelSetPackage->getData();

            ChunkCoord chunkCoord = voxelToChunk(voxelCoord);
            ChunkVoxelCoord chunkVoxelCoord = voxelToChunkVoxel(voxelCoord);

            auto chunk = mLocalChunks.find(chunkCoord);

            if(chunk != mLocalChunks.end())
            {
                chunk->second.setVoxelType(chunkVoxelCoord, type);
                updateChunk(chunkCoord);
            }

            ChunkCoord leftNeighbour = ChunkCoord(chunkCoord.x - 1, chunkCoord.y, chunkCoord.z);
            if(chunkVoxelCoord.x == 0 && mLocalChunks.find(leftNeighbour) != mLocalChunks.end())
            {
                updateChunk(leftNeighbour);
            }

            ChunkCoord rightNeighbour = ChunkCoord(chunkCoord.x + 1, chunkCoord.y, chunkCoord.z);
            if(chunkVoxelCoord.x == chunkWidth - 1 && mLocalChunks.find(rightNeighbour) != mLocalChunks.end())
            {
                updateChunk(rightNeighbour);
            }

            ChunkCoord topNeighbour = ChunkCoord(chunkCoord.x, chunkCoord.y + 1, chunkCoord.z);
            if(chunkVoxelCoord.y == chunkWidth - 1 && mLocalChunks.find(topNeighbour) != mLocalChunks.end())
            {
                updateChunk(topNeighbour);
            }

            ChunkCoord bottomNeighbour = ChunkCoord(chunkCoord.x, chunkCoord.y - 1, chunkCoord.z);
            if(chunkVoxelCoord.y == 0 && mLocalChunks.find(bottomNeighbour) != mLocalChunks.end())
            {
                updateChunk(bottomNeighbour);
            }

            ChunkCoord frontNeighbour = ChunkCoord(chunkCoord.x, chunkCoord.y, chunkCoord.z + 1);
            if(chunkVoxelCoord.z == chunkWidth - 1 && mLocalChunks.find(frontNeighbour) != mLocalChunks.end())
            {
                updateChunk(frontNeighbour);
            }

            ChunkCoord backNeighbour = ChunkCoord(chunkCoord.x, chunkCoord.y, chunkCoord.z - 1);
            if(chunkVoxelCoord.z == 0 && mLocalChunks.find(backNeighbour) != mLocalChunks.end())
            {
                updateChunk(backNeighbour);
            }
        }
		else if(package->mType == PackageType::CHUNK_DELETED)
		{
			ChunkDeletedPackage* chunkPackage = (ChunkDeletedPackage*)package.get();

            ChunkCoord coordinate;

            std::tie(coordinate) = chunkPackage->getData();

			mBus.send(ClientChunkDeletedMessage{std::get<0>(chunkPackage->getData())});

            mLocalChunks.erase(coordinate);
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

			mBus.send<PlayerFacingBlockMessage>(PlayerFacingBlockMessage{playerId, VoxelCoord(x, y, z)});
		}
	}
}

void Client::updateChunk(const ChunkCoord& coordinate)
{
    Chunk* mainChunk = &mLocalChunks.at(coordinate);
    Chunk* topChunk = nullptr;
    Chunk* bottomChunk = nullptr;
    Chunk* frontChunk = nullptr;
    Chunk* backChunk = nullptr;
    Chunk* leftChunk = nullptr;
    Chunk* rightChunk = nullptr;

    auto top    = mLocalChunks.find(ChunkCoord(coordinate.x, coordinate.y + 1, coordinate.z));
    auto bottom = mLocalChunks.find(ChunkCoord(coordinate.x, coordinate.y - 1, coordinate.z));
    auto front  = mLocalChunks.find(ChunkCoord(coordinate.x, coordinate.y, coordinate.z + 1));
    auto back   = mLocalChunks.find(ChunkCoord(coordinate.x, coordinate.y, coordinate.z - 1));
    auto left   = mLocalChunks.find(ChunkCoord(coordinate.x - 1, coordinate.y, coordinate.z));
    auto right  = mLocalChunks.find(ChunkCoord(coordinate.x + 1, coordinate.y, coordinate.z));

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

    mBus.send<UpdateChunkVboMessage>(UpdateChunkVboMessage{mainChunk, topChunk, bottomChunk, frontChunk, backChunk, leftChunk, rightChunk});
}
