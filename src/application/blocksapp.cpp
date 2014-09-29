#include "blocksapp.hpp"
#include "../input/inputactions.hpp"
#include "../networking/localserverclientbridge.hpp"
#include "../networking/remoteserverbridge.hpp"
#include "../networking/localclientconnectionlistener.hpp"
#include "../networking/remoteclientconnectionlistener.hpp"
#include "../networking/networkparameters.hpp"
#include <iostream>
//#include <thread>

BlocksApplication::BlocksApplication()
{
    subscribe(mClientBus, *this, false);
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
			// server->getBus().send(LogMessage{"No address given. Good bye.", "Client", LogLevel::INFO});
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
        client->update();
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
        client = std::unique_ptr<Client>();
    }
    if(server)
    {
        server = std::unique_ptr<Server>();
    }
}

void BlocksApplication::handleMessage(const LocalConnectionAttemptMessage& received)
{
    mServerBus.send(received);
}

void BlocksApplication::setupSinglePlayer()
{
    NetworkParameters parameters;

    parameters.mode = NetworkMode::SINGLE_PLAYER;
    server = std::unique_ptr<Server>(new Server(mServerBus, parameters));
    client = std::unique_ptr<Client>(new Client(mClientBus, parameters));
}

void BlocksApplication::setupMultiPlayer()
{
    //server = std::unique_ptr<Server>(new Server(mServerBus));
	//client = std::unique_ptr<Client>(new Client(mClientBus));

    //RemoteServerBridge* serverToClient = new RemoteServerBridge(mServerBus);
  	//RemoteServerBridge* clientToServer = new RemoteServerBridge(mClientBus);

	//serverToClient->startListening();

    //client->setServerBridge(std::unique_ptr<RemoteServerBridge>(clientToServer));
	//clientToServer->connectToAddress("localhost");
}

void BlocksApplication::setupDedicatedServer(int32_t port)
{
    //server = std::unique_ptr<Server>(new Server(mServerBus));

    //RemoteClientConnectionListener* remoteListener = new RemoteClientConnectionListener(mServerBus);

    //if(enet_initialize() < 0)
    //{
    //  mServerBus.send(LogMessage{"Couldn't initialise enet", "network", LogLevel::ERR});
    //}
    //else
    //{
    //  remoteListener->startListening(port);
    //  server->setClientListener(std::unique_ptr<RemoteClientConnectionListener>(remoteListener));
    //}
}

void BlocksApplication::joinServer(const std::string& address, int32_t port)
{
	//client = std::unique_ptr<Client>(new Client(mClientBus));
  	//RemoteServerBridge* serverBidge = new RemoteServerBridge(mClientBus);

    //if(enet_initialize() < 0)
    //{
    //  mClientBus.send(LogMessage{"Couldn't initialise enet", "network", LogLevel::ERR});
    //}
    //else
    //{
    //  client->setServerBridge(std::unique_ptr<RemoteServerBridge>(serverBidge));
    //  serverBidge->connectToAddress(address, port);
    //  serverBidge->startListening();
    //}
}
