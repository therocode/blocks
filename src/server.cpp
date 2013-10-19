#include "server.h"
#include "packages.h"
#include <iostream>

Server::Server() : mWorld(mBus),
                   mLogger(mBus),
                   mBridge(nullptr),
                   mScriptHandler(mBus, mWorld.getWorldInterface())
{
    mBus.addMessageSubscriber<ChunkCreatedMessage>(*this);
    mBus.addMessageSubscriber<AddGfxEntityMessage>(*this);
    mBus.addMessageSubscriber<MoveGfxEntityMessage>(*this);
    mBus.addMessageSubscriber<RemoveGfxEntityMessage>(*this);
}

Server::~Server()
{
    mBus.removeMessageSubscriber<ChunkCreatedMessage>(*this);
    mBus.removeMessageSubscriber<AddGfxEntityMessage>(*this);
    mBus.removeMessageSubscriber<MoveGfxEntityMessage>(*this);
    mBus.removeMessageSubscriber<RemoveGfxEntityMessage>(*this);
}

void Server::setup()
{
    mScriptHandler.setup();
    mWorld.initialise();
    mFrameTimer.start();
    std::cout << "Server initialised and ready to go\n";
}

void Server::doLogic()
{
    fetchClientData();

    mBus.sendMessage<FrameMessage>(FrameMessage(true));

    mWorld.update();

    mBridge->flush();
    mFrameTimer.sleepForTheRestOfTheFrame();
    mFrameTimer.start();
}

void Server::destroy()
{
    mScriptHandler.destroy();
    std::cout << "Server destroyed\n";
}

void Server::addClientBridge(std::unique_ptr<ServerClientBridge> clientBridge)
{
    mBridge = std::move(clientBridge);
    std::cout << "Server got a client connected\n";
}

void Server::handleMessage(const ChunkCreatedMessage& received)
{
	ChunkCoordinate coordinate;
	VoxelTypeArray types;

	std::tie(coordinate, types) = received.data;

    mBridge->enqueuePackage(std::unique_ptr<BasePackage>(new ChunkLoadedPackage(coordinate, types)));
}

void Server::handleMessage(const AddGfxEntityMessage& received)
{
    size_t id;
    glm::vec3 position;

    std::tie(id, position) = received.data;

    mBridge->enqueuePackage(std::unique_ptr<BasePackage>(new GfxEntityAddedPackage(id, position)));
}

void Server::handleMessage(const MoveGfxEntityMessage& received)
{
    size_t id;
    glm::vec3 position;

    std::tie(id, position) = received.data;

    mBridge->enqueuePackage(std::unique_ptr<BasePackage>(new GfxEntityMovedPackage(id, position)));
}

void Server::handleMessage(const RemoveGfxEntityMessage& received)
{
    size_t id;

    std::tie(id) = received.data;

    mBridge->enqueuePackage(std::unique_ptr<BasePackage>(new GfxEntityRemovedPackage(id)));
}

void Server::fetchClientData()
{
    std::unique_ptr<BasePackage> package;

    while(mBridge->pollPackage(package))
    {
        if(package->mType == typeid(RebuildScriptsRequestedPackage))
        {
            mBus.sendMessage<RebuildScriptsRequestedMessage>(RebuildScriptsRequestedMessage('0'));
        }
    }
}
