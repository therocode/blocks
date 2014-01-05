#include "blocksapp.h"
#include "../input/inputactions.h"
#include "../networking/localserverclientbridge.h"
#include "../networking/remoteserverbridge.h"
#include "../networking/localclientconnectionlistener.h"
#include "../networking/remoteclientconnectionlistener.h"
#include <iostream>
#include <thread>

BlocksApplication::BlocksApplication()
{
}

void BlocksApplication::setup(const std::vector<std::string>& args)
{
    if(args.size() <= 1)
    {
        setupSinglePlayer();	//setup local server and local client and connect with LocalServerClientBridge
    }
    else if(args[1] == "--dedicated" || args[1] == "dedicated")
    {
        setupDedicatedServer(56566);  //do not create a client and only initialise a server and give it a NetworkServerClientBridge
    }
    else if(args[1] == "--join" || args[1] == "join")
    {
		if(args.size() > 2)
		{
			std::string address = args[2];
			int32_t port = -1;
			if(args.size() > 3)
			{
				port = std::stoi(args[3]);
			}
			joinServer(address, port);   //setup local client and do not setup a server. give client a NetworkServerClientBridge and connect it to remote
		}else{
		//sergveri isn't craeted
			// server->getBus().send<LogMessage>(LogMessage("No address given. Good bye.", "Client", LogLevel::INFO));
			printf("No server address specified. Quitting.\n");
			exit(3);
		}
	}
	else if(args[1] == "--host" || args[1] == "host")
	{
		setupMultiPlayer();
	}
    else
    {
        std::cout << "Error! Malformed input! Must start ethier without arguments, or with one of the following: \"--dedicated\"  \"--join <server_address> <port_number>\"\n";
        exit(1);
    }
}

void BlocksApplication::loop()
{
    if(client)
    {
        client->handleInput();
    }

    if(server)
    {
        server->doLogic();
    }

    if(client)
    {
        client->render();
    }

    if(client)
    {
        if(client->requestedQuit())
            quit();
    }
}

void BlocksApplication::destroy()
{
    if(client)
    {
        client->destroy();
    }
    if(server)
    {
        server->destroy();
    }
}

void BlocksApplication::setupSinglePlayer()
{
    server = std::unique_ptr<Server>(new Server());
    client = std::unique_ptr<Client>(new Client());

    //setup server and client separately
    server->setup();
    client->setup();

    //setup client and server local connection
    LocalServerClientBridge* clientToServer = new LocalServerClientBridge();
    LocalClientConnectionListener* localListener = new LocalClientConnectionListener();

    server->setClientListener(std::unique_ptr<LocalClientConnectionListener>(localListener));
    client->setServerBridge(std::unique_ptr<LocalServerClientBridge>(clientToServer));
    localListener->createClientConnection(clientToServer);
}

void BlocksApplication::setupMultiPlayer()
{
    //server = std::unique_ptr<Server>(new Server());
	//client = std::unique_ptr<Client>(new Client());

    //RemoteServerClientBridge* serverToClient = new RemoteServerClientBridge(true);
  	//RemoteServerClientBridge* clientToServer = new RemoteServerClientBridge(false);

	//serverToClient->startListening();

    //server->setup();

    //client->setServerBridge(std::unique_ptr<RemoteServerClientBridge>(clientToServer));
	//client->setup();
	//clientToServer->connectToAddress("localhost");
}

void BlocksApplication::setupDedicatedServer(int32_t port)
{
    server = std::unique_ptr<Server>(new Server());

    server->setup();
    RemoteClientConnectionListener* remoteListener = new RemoteClientConnectionListener(server->getBus());

    if(enet_initialize() < 0)
    {
        server->getBus().send<LogMessage>(LogMessage("Couldn't initialise enet", "network", LogLevel::ERR));
    }
    else
    {
        remoteListener->startListening(port);
        server->setClientListener(std::unique_ptr<RemoteClientConnectionListener>(remoteListener));
    }
}

void BlocksApplication::joinServer(const std::string& address, int32_t port)
{
	client = std::unique_ptr<Client>(new Client());
  	RemoteServerBridge* serverBidge = new RemoteServerBridge(client->getBus());

    if(enet_initialize() < 0)
    {
        client->getBus().send<LogMessage>(LogMessage("Couldn't initialise enet", "network", LogLevel::ERR));
    }
    else
    {
        client->setServerBridge(std::unique_ptr<RemoteServerBridge>(serverBidge));
        serverBidge->connectToAddress(address, port);
        serverBidge->startListening();
    }
	client->setup();
}
