#include "server.h"

Server::Server() : world(mBus),
                   mBridge(nullptr)
{
}

void Server::setup()
{
    world.initialise();
}

void Server::doLogic()
{
    //fetch client stuff

    world.update();

    mBridge->flush();
}

void Server::destroy()
{
}

void Server::addClientBridge(std::unique_ptr<ServerClientBridge> clientBridge)
{
    mBridge = std::move(clientBridge);
}
