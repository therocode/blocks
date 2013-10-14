#include "server.h"
#include "chunkloadedpackage.h"
#include <iostream>

Server::Server() : world(mBus),
                   mBridge(nullptr)
{
    mBus.addMessageSubscriber<ChunkCreatedMessage>(*this);
}

Server::~Server()
{
    mBus.removeMessageSubscriber<ChunkCreatedMessage>(*this);
}

void Server::setup()
{
    world.initialise();
    std::cout << "Server initialised and ready to go\n";
}

void Server::doLogic()
{
    //fetch client stuff

    world.update();

    mBridge->flush();
}

void Server::destroy()
{
    std::cout << "Server destroyed\n";
}

void Server::addClientBridge(std::unique_ptr<ServerClientBridge> clientBridge)
{
    mBridge = std::move(clientBridge);
    std::cout << "Server got a client connected\n";
}

void Server::handleMessage(const ChunkCreatedMessage& received)
{
    std::cout << "Server has created a chunk and will send it to the client\n";
    
	const ChunkCoordinate* coordinate;
	const Chunk* chunk;

	std::tie(coordinate, chunk) = received.data;

    mBridge->enqueuePackage(std::unique_ptr<Package>(new ChunkLoadedPackage(*coordinate, *chunk)));
}
