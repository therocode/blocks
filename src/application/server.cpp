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

    pollNewClients();

    mFrameTimer.sleepForTheRestOfTheFrame();
    mFrameTimer.start();
}

void Server::destroy()
{
    mScriptHandler.destroy();
    mBus.sendMessage<LogMessage>(LogMessage("Server destroyed", mLogName));
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

    graphicsEntities.insert(id); //temphack
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

    graphicsEntities.erase(id); //temphack
}

void Server::setClientListener(std::unique_ptr<ClientConnectionListener> clientListener)
{
    mListener = std::move(clientListener);
}

void Server::acceptClientConnection(std::shared_ptr<ClientConnection> client)
{
    ClientId newClientId = client->getId();

    mClients.emplace(newClientId, client);

    mBus.sendMessage<LogMessage>(LogMessage(std::string("Client id ") + std::to_string(newClientId) + std::string(" connected"), mLogName));

    //resend current gfx entities. this is a hack right now. in the futuer it probably has to send the whole game state or something, i dunno
    for(size_t id : graphicsEntities)
    {
        std::shared_ptr<BasePackage> gfxEntityAddedPackage(new GfxEntityAddedPackage(id, glm::vec3(0.0f, 0.0f, 0.0f)));
        client->enqueuePackage(gfxEntityAddedPackage);
    }

    //resend chunk created messages
    for(const auto& chunk : mWorld.getWorldInterface().getChunkList())
    {
        std::shared_ptr<BasePackage> chunkAddedPackage(new ChunkLoadedPackage(chunk.getLocation(), chunk.getVoxelTypes()));
        client->enqueuePackage(chunkAddedPackage);
    }

    //create entity
}

void Server::pollNewClients()
{
    std::shared_ptr<ClientConnection> client;
    
    if(mListener)
    {
        while((client = mListener->fetchIncomingConnection()))
        {
            acceptClientConnection(client);
        }
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
