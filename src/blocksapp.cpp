#include "blocksapp.h"
#include "input/inputactions.h"
#include "localserverclientbridge.h"
#include <iostream>
#include <thread>

BlocksApplication::BlocksApplication()
{
}

void BlocksApplication::setup(const std::vector<std::string>& args)
{
    if(args.size() <= 1)
    {
        setupSinglePlayer(); //setup local server and local client and connect with LocalServerClientBridge
    }
    else if(args[1] == "--dedicated")
    {
        setupDedicatedServer();  //do not create a client and only initialise a server and give it a NetworkServerClientBridge
    }
    else if(args[1] == "--join")
    {
        std::string address = args[2];
        int32_t port = std::stoi(args[3]);
        joinServer(address, port);   //setup local client and do not setup a server. give client a NetworkServerClientBridge and connect it to remote
    }
    else
    {
        std::cout << "Error! Malformed input! Must start ethier without arguments, or with one of the following: \"--dedicated\"  \"--join <server_address> <port_number>\"\n";
        exit(1);
    }
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

void BlocksApplication::setupSinglePlayer()
{
    std::cout << "Initialising single player game with a local server and a local client\n";
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

void BlocksApplication::setupDedicatedServer()
{
    std::cout << "Initialising dedicated server without a local client\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "CRAHSHCA SEGMENTATION FAULT HIHIHI!\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    std::cout << "No, just kidding, not implemented yet\n";
    exit(1);
}

void BlocksApplication::joinServer(const std::string& address, int32_t port)
{
    std::cout << "Initialising single player game with a local server and a local client\n";
    std::cout << "Joining server " << address << " on port " << port << "...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "fatal error! You have been permanently banned!\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    std::cout << "No, just kidding, not implemented yet\n";
    exit(1);
}
