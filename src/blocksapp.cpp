#include "blocksapp.h"
#include "input/inputactions.h"

BlocksApplication::BlocksApplication()
{
}

void BlocksApplication::setup()
{
    server = std::unique_ptr<Server>(new Server());
    client = std::unique_ptr<Client>(new Client());
    server->setup();
    client->setup();
}

void BlocksApplication::loop()
{
    client->handleInput();
    server->doLogic(); //implement timestep later on. should also only happen if the server is a local instance
    client->render();

    if(client->requestedQuit())
        quit();
}

void BlocksApplication::destroy()
{
    client->destroy();
    server->destroy();
}
