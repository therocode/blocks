#include "blocksapp.h"
#include "input/inputactions.h"
#include "localserverclientbridge.h"

BlocksApplication::BlocksApplication()
{
}

void BlocksApplication::setup()
{
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
#ifdef __UNIX__
	printf("HEJJJ UNIX\n");
#endif
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
