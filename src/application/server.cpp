#include "server.h"
#include "../networking/packages.h"

Server::Server() : mWorld(mBus),
                   mLogger(mBus),
                   mScriptHandler(mBus, mWorld.getWorldInterface()),
                   mLogName("server")
{
    mBus.addMessageSubscriber<ChunkCreatedMessage>(*this);
    mBus.addMessageSubscriber<AddGfxEntityMessage>(*this);
    mBus.addMessageSubscriber<MoveGfxEntityMessage>(*this);
    mBus.addMessageSubscriber<RemoveGfxEntityMessage>(*this);
}

Server::~Server()
{
    mBus.removeMessageSubscriber<ChunkCreatedMessage>(*this);
    mBus.removeMessageSubscriber<AddGfxEntityMessage>(*this);
    mBus.removeMessageSubscriber<MoveGfxEntityMessage>(*this);
    mBus.removeMessageSubscriber<RemoveGfxEntityMessage>(*this);
}

void Server::setup()
{
    mScriptHandler.setup();
    mWorld.initialise();
    mFrameTimer.start();
    mBus.sendMessage<LogMessage>(LogMessage("Server initialised and ready to go", mLogName));
}

void Server::doLogic()
{
    for(auto& client : mClients)
    {
        fetchClientData(client.second);
    }

    mBus.sendMessage<FrameMessage>(FrameMessage(true));

    mWorld.update();

    for(auto& client : mClients)
    {
        client.second->flush();
    }
    mFrameTimer.sleepForTheRestOfTheFrame();
    mFrameTimer.start();
}

void Server::destroy()
{
    mScriptHandler.destroy();
    mBus.sendMessage<LogMessage>(LogMessage("Server destroyed", mLogName));
}

void Server::addClientBridge(std::unique_ptr<ServerClientBridge> clientBridge)
{
    //mBridge = std::move(clientBridge);
    //mBus.sendMessage<LogMessage>(LogMessage("Server got a client connected", mLogName));

    ////client will probably get some kind of player ID;
    //size_t playerId = 0;

    ////Load previous position or choose a spawn position if it is the first time a player connects
    //glm::vec3 spawnPos(0.0f, -50.0f, 0.0f);

    //mBus.sendMessage<PlayerJoinedMessage>(PlayerJoinedMessage(playerId, spawnPos));
}

void Server::handleMessage(const ChunkCreatedMessage& received)
{
	ChunkCoordinate coordinate;
	VoxelTypeArray types;

	std::tie(coordinate, types) = received.data;

    std::shared_ptr<BasePackage> chunkLoadedPackage(new ChunkLoadedPackage(coordinate, types));
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(chunkLoadedPackage);
    }
}

void Server::handleMessage(const AddGfxEntityMessage& received)
{
    size_t id;
    glm::vec3 position;

    std::tie(id, position) = received.data;

    std::shared_ptr<BasePackage> gfxEntityAddedPackage(new GfxEntityAddedPackage(id, position));
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(gfxEntityAddedPackage);
    }
}

void Server::handleMessage(const MoveGfxEntityMessage& received)
{
    size_t id;
    glm::vec3 position;

    std::tie(id, position) = received.data;

    std::shared_ptr<BasePackage> gfxEntityMovedPackage(new GfxEntityMovedPackage(id, position));
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(gfxEntityMovedPackage);
    }
}

void Server::handleMessage(const RemoveGfxEntityMessage& received)
{
    size_t id;

    std::tie(id) = received.data;

    std::shared_ptr<BasePackage> gfxEntityRemovedPackage(new GfxEntityRemovedPackage(id));
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(gfxEntityRemovedPackage);
    }
}

void Server::acceptClientConnection(std::shared_ptr<ClientConnection> client)
{
    ClientId newClientId = client->getId();

    mClients.emplace(newClientId, client);

}

void Server::pollNewClients()
{
    std::shared_ptr<ClientConnection> client;

    while((client = mListener->fetchIncomingConnection()))
    {
        acceptClientConnection(client);
    }
}

void Server::fetchClientData(std::weak_ptr<ClientConnection> client)
{
    std::shared_ptr<BasePackage> package;

    for(auto& client : mClients)
    {
        while(client.second->pollPackage(package))
        {
            if(package->mType == typeid(RebuildScriptsRequestedPackage))
            {
                mBus.sendMessage<RebuildScriptsRequestedMessage>(RebuildScriptsRequestedMessage('0'));
            }
        }
    }
}
