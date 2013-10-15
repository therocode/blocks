#include "blocksapp.h"
#include "input/inputactions.h"
#include "localserverclientbridge.h"
#include <iostream>

BlocksApplication::BlocksApplication()
{
}

void BlocksApplication::setup(const std::vector<std::string>& args)
{
    if(!args.size())
    {
        setupSinglePlayer(); //setup local server and local client and connect with LocalServerClientBridge
    }
    else if(args[0] == "--dedicated")
    {
        setupDedicatedServer();  //do not create a client and only initialise a server and give it a NetworkServerClientBridge
    }
    else if(args[0] == "--join")
    {
        std::string address = args[1];
        int32_t port = std::stoi(args[2]);
        joinServer(address, port);   //setup local client and do not setup a server. give client a NetworkServerClientBridge and connect it to remote
    }

    server = std::unique_ptr<Server>(new Server());
    client = std::unique_ptr<Client>(new Client());

    LocalServerClientBridge* clientToServer = new LocalServerClientBridge();
    LocalServerClientBridge* serverToClient = new LocalServerClientBridge();

    clientToServer->connect(serverToClient);
    serverToClient->connect(clientToServer);

    client->setServerBridge(std::unique_ptr<LocalServerClientBridge>(clientToServer));
    server->addClientBridge(std::unique_ptr<LocalServerClientBridge>(serverToClient));

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

void BlocksApplication::setupSinglePlayer()
{
}

void BlocksApplication::setupDedicatedServer()
{
}

void BlocksApplication::joinServer(const std::string& address, int32_t port)
{
}
