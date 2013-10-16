#include "server.h"
#include "chunkloadedpackage.h"
#include "gfxentityaddedpackage.h"
#include "gfxentitymovedpackage.h"
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
    //fetch client stuff

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
	const ChunkCoordinate* coordinate;
	const Chunk* chunk;

	std::tie(coordinate, chunk) = received.data;

    mBridge->enqueuePackage(std::unique_ptr<Package>(new ChunkLoadedPackage(*coordinate, *chunk)));
}

void Server::handleMessage(const AddGfxEntityMessage& received)
{
    size_t id;
    glm::vec3 position;

    std::tie(id, position) = received.data;

    mBridge->enqueuePackage(std::unique_ptr<Package>(new GfxEntityAddedPackage(id, position)));
}

void Server::handleMessage(const MoveGfxEntityMessage& received)
{
    size_t id;
    glm::vec3 position;

    std::tie(id, position) = received.data;

    mBridge->enqueuePackage(std::unique_ptr<Package>(new GfxEntityMovedPackage(id, position)));
}
