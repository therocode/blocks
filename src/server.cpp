#include "server.h"
#include <iostream>

Server::Server() : world(mBus),
                   mBridge(nullptr)
{
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
