#include "blocksapp.hpp"
#include "../networking/networkparameters.hpp"
#include <iostream>

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
    else if(args[1] == "--dedicated" || args[1] == "-d")
    {
        setupDedicatedServer(56566);  //do not create a client and only initialise a server and give it a NetworkServerClientBridge
    }
    else if(args[1] == "--join" || args[1] == "-d")
    {
		if(args.size() > 2)
		{
			const std::string& address = args[2];
			int32_t port = -1;
			if(args.size() > 3)
			{
				port = std::stoi(args[3]);
			}
			joinServer(address, port);   //setup local client and do not setup a server. give client a NetworkServerClientBridge and connect it to remote
		}
        else
        {
            std::cout << "No server address specified. Quitting.\n";
			exit(3);
		}
	}
	else if(args[1] == "--host" || args[1] == "-h")
	{
		setupMultiPlayer(56566);
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

#if !defined(CLIENT_ONLY)
    if(server)
    {
        server->doLogic();
    }
#endif

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
#if !defined(CLIENT_ONLY)
    if(server)
    {
        server = std::unique_ptr<Server>();
    }
#endif
}

void BlocksApplication::handleMessage(const LocalConnectionAttemptMessage& received)
{
#if !defined(CLIENT_ONLY)
    mServerBus.send(received);
#endif
}

void BlocksApplication::setupSinglePlayer()
{
    NetworkParameters parameters;

    parameters.mode = NetworkMode::SINGLE_PLAYER;
#if !defined(CLIENT_ONLY)
    server = std::unique_ptr<Server>(new Server(mServerBus, parameters));
#endif
    client = std::unique_ptr<Client>(new Client(mClientBus, parameters));
}

void BlocksApplication::setupMultiPlayer(int32_t port)
{
    NetworkParameters parameters;

    parameters.mode = NetworkMode::COMBINED;
    parameters.port = port;

#if !defined(CLIENT_ONLY)
    server = std::unique_ptr<Server>(new Server(mServerBus, parameters));
#endif
	client = std::unique_ptr<Client>(new Client(mClientBus, parameters));
}

void BlocksApplication::setupDedicatedServer(int32_t port)
{
    NetworkParameters parameters;

    parameters.mode = NetworkMode::DEDICATED;
    parameters.port = port;
#if !defined(CLIENT_ONLY)
    server = std::unique_ptr<Server>(new Server(mServerBus, parameters));
#endif
}

void BlocksApplication::joinServer(const std::string& address, int32_t port)
{
    NetworkParameters parameters;

    parameters.mode = NetworkMode::JOIN;
    parameters.address = address;
    parameters.port = port;

    client = std::unique_ptr<Client>(new Client(mClientBus, parameters));
}
