#include "server.h"

Server::Server() : world(mBus)
{
}

void Server::setup()
{
    world.initialise();
}

void Server::doLogic()
{
    world.update();
}

void Server::destroy()
{
}
