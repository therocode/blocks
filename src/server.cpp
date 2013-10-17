#include "server.h"
#include "packages.h"
#include <iostream>

Server::Server() : mWorld(mBus),
                   mBridge(nullptr),
                   mScriptHandler(mBus)
{
    mBus.addMessageSubscriber<ChunkCreatedMessage>(*this);
    mBus.addMessageSubscriber<AddGfxEntityMessage>(*this);
    mBus.addMessageSubscriber<MoveGfxEntityMessage>(*this);
}

Server::~Server()
{
    mBus.removeMessageSubscriber<ChunkCreatedMessage>(*this);
    mBus.removeMessageSubscriber<AddGfxEntityMessage>(*this);
    mBus.removeMessageSubscriber<MoveGfxEntityMessage>(*this);
}

void Server::setup()
{
    mScriptHandler.setup();
    mWorld.initialise();
    std::cout << "Server initialised and ready to go\n";
}

void Server::doLogic()
{
    fetchClientData();

    mBus.sendMessage<FrameMessage>(FrameMessage(true));

    mWorld.update();

    mBridge->flush();
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
