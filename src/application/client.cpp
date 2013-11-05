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
}

Client::~Client()
{
	mBus.removeMessageSubscriber<PlayerActionMessage>(*this);
	mBus.removeMessageSubscriber<PlayerMoveDirectionMessage>(*this);
	mBus.removeMessageSubscriber<PlayerMoveActionMessage>(*this);
	mBus.removeMessageSubscriber<PlayerPitchYawMessage>(*this);
	mBus.removeMessageSubscriber<RebuildScriptsRequestedMessage>(*this);
}

void Client::setup()
{
	
	mWindow.create(fea::VideoMode(800, 600, 32), "Blocky", fea::Style::Default, fea::ContextSettings(32));
	mWindow.lockCursor(true);
	// mWindow.setVSyncEnabled(false);
	mRenderer.setup();
	//	mWindow.setFramerateLimit(30);

	mBus.sendMessage<WindowResizeMessage>(WindowResizeMessage(800, 600));
	// mWindow.lockCursor(true);
	//    std::cout << "client setup\n";
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
		mBridge->enqueuePackage(std::shared_ptr<BasePackage>(new PlayerActionPackage(playerId, action)));
	}
}

void Client::handleMessage(const PlayerMoveDirectionMessage& received)
{
    size_t id;
    MoveDirection dir;

    std::tie(id, dir) = received.data;

    mBridge->enqueuePackage(std::shared_ptr<BasePackage>(new PlayerMoveDirectionPackage(id, dir.getForwardBack(), dir.getLeftRight())));
}

void Client::handleMessage(const PlayerMoveActionMessage& received)
{
    mBridge->enqueuePackage(std::shared_ptr<BasePackage>(new PlayerMoveActionPackage(received.data)));
}

void Client::handleMessage(const PlayerPitchYawMessage& received)
{
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

bool Client::requestedQuit()
{
	return mQuit;
}

void Client::setServerBridge(std::unique_ptr<ServerClientBridge> bridge)
{
	mBridge = std::move(bridge);
	mBus.sendMessage<LogMessage>(LogMessage("client connected to server", mLogName, LogLevel::INFO));
}

void Client::fetchServerData()
{
	std::shared_ptr<BasePackage> package;
	while(mBridge->pollPackage(package))
	{
		if(package->mType == typeid(ChunkLoadedPackage))
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
		else if(package->mType == typeid(ChunkDeletedPackage))
		{
			ChunkDeletedPackage* chunkPackage = (ChunkDeletedPackage*)package.get();

            ChunkCoordinate coordinate;

            std::tie(coordinate) = chunkPackage->getData();

			mBus.sendMessage<ChunkDeletedMessage>(ChunkDeletedMessage(chunkPackage->getData()));

            mLocalChunks.erase(coordinate);
		}
		else if(package->mType == typeid(GfxEntityAddedPackage))
		{
			GfxEntityAddedPackage* gfxAddedPackage = (GfxEntityAddedPackage*)package.get();
			mBus.sendMessage<AddGfxEntityMessage>(AddGfxEntityMessage(gfxAddedPackage->getData()));
		}
		else if(package->mType == typeid(GfxEntityMovedPackage))
		{
			GfxEntityMovedPackage* gfxMovedPackage = (GfxEntityMovedPackage*)package.get();
			mBus.sendMessage<MoveGfxEntityMessage>(MoveGfxEntityMessage(gfxMovedPackage->getData()));
		}
		else if(package->mType == typeid(GfxEntityRotatedPackage))
		{
			GfxEntityRotatedPackage* gfxEntityRotatedPackage = (GfxEntityRotatedPackage*)package.get();
			mBus.sendMessage<RotateGfxEntityMessage>(RotateGfxEntityMessage(gfxEntityRotatedPackage->getData()));
		}
		else if(package->mType == typeid(GfxEntityRemovedPackage))
		{
			GfxEntityRemovedPackage* gfxRemovedPackage = (GfxEntityRemovedPackage*)package.get();
			mBus.sendMessage<RemoveGfxEntityMessage>(RemoveGfxEntityMessage(gfxRemovedPackage->getData()));
		}
		else if(package->mType == typeid(PlayerIdPackage))
		{
			PlayerIdPackage* playerIdPackage = (PlayerIdPackage*)package.get();
			mBus.sendMessage<PlayerIdMessage>(PlayerIdMessage(playerIdPackage->getData()));
		}
		else if(package->mType == typeid(PlayerConnectedToEntityPackage))
		{
			PlayerConnectedToEntityPackage* playerConnectedToEntityPackage = (PlayerConnectedToEntityPackage*)package.get();
			mBus.sendMessage<PlayerConnectedToEntityMessage>(PlayerConnectedToEntityMessage(playerConnectedToEntityPackage->getData()));
		}
		else if(package->mType == typeid(PlayerFacingBlockPackage))
		{
			PlayerFacingBlockPackage* playerFacingBlockPackage = (PlayerFacingBlockPackage*)package.get();

            size_t playerId;

            float x;
            float y;
            float z;

            std::tie(playerId, x, y, z) = playerFacingBlockPackage->getData();

			mBus.sendMessage<PlayerFacingBlockMessage>(PlayerFacingBlockMessage(playerId, glm::vec3(x, y, z)));
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
    mBus.sendMessage<UpdateChunkVboMessage>(UpdateChunkVboMessage(mainChunk, topChunk, bottomChunk, frontChunk, backChunk, leftChunk, rightChunk));
}
