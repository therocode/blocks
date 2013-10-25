#include "server.h"
#include "../networking/packages.h"

Server::Server() : mWorld(mBus),
                   mLogger(mBus),
                   mScriptHandler(mBus, mWorld.getWorldInterface()),
                   mLogName("server")
{
    mBus.addMessageSubscriber<ChunkCreatedMessage>(*this);
    mBus.addMessageSubscriber<ChunkDeletedMessage>(*this);
    mBus.addMessageSubscriber<AddGfxEntityMessage>(*this);
    mBus.addMessageSubscriber<MoveGfxEntityMessage>(*this);
    mBus.addMessageSubscriber<RotateGfxEntityMessage>(*this);
    mBus.addMessageSubscriber<RemoveGfxEntityMessage>(*this);
    mBus.addMessageSubscriber<PlayerConnectedToEntityMessage>(*this);
    mBus.addMessageSubscriber<PlayerFacingBlockMessage>(*this);
}

Server::~Server()
{
    mBus.removeMessageSubscriber<ChunkCreatedMessage>(*this);
    mBus.removeMessageSubscriber<ChunkDeletedMessage>(*this);
    mBus.removeMessageSubscriber<AddGfxEntityMessage>(*this);
    mBus.removeMessageSubscriber<MoveGfxEntityMessage>(*this);
    mBus.removeMessageSubscriber<RotateGfxEntityMessage>(*this);
    mBus.removeMessageSubscriber<RemoveGfxEntityMessage>(*this);
    mBus.removeMessageSubscriber<PlayerConnectedToEntityMessage>(*this);
    mBus.removeMessageSubscriber<PlayerFacingBlockMessage>(*this);
}

void Server::setup()
{
    mScriptHandler.setup();
    mWorld.initialise();
	mFrameTimer.setDesiredFPSRate(144);
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

   // mFrameTimer.sleepForTheRestOfTheFrame();
    //mFrameTimer.start();
}

void Server::destroy()
{
    mScriptHandler.destroy();
    mBus.sendMessage<LogMessage>(LogMessage("Server destroyed", mLogName));
}

void Server::handleMessage(const ChunkCreatedMessage& received)
{
    std::shared_ptr<BasePackage> chunkLoadedPackage(new ChunkLoadedPackage(received.data));
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(chunkLoadedPackage);
    }
}

void Server::handleMessage(const ChunkDeletedMessage& received)
{
    std::shared_ptr<BasePackage> chunkDeletedPackage(new ChunkDeletedPackage(received.data));
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(chunkDeletedPackage);
    }
}

void Server::handleMessage(const AddGfxEntityMessage& received)
{
    size_t id;

    std::tie(id, std::ignore) = received.data;

    std::shared_ptr<BasePackage> gfxEntityAddedPackage(new GfxEntityAddedPackage(received.data));
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(gfxEntityAddedPackage);
    }

    graphicsEntities.insert(id); //temphack
}

void Server::handleMessage(const MoveGfxEntityMessage& received)
{
    std::shared_ptr<BasePackage> gfxEntityMovedPackage(new GfxEntityMovedPackage(received.data));
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(gfxEntityMovedPackage);
    }
}

void Server::handleMessage(const RotateGfxEntityMessage& received)
{
    std::shared_ptr<BasePackage> gfxEntityRotatedPackage(new GfxEntityRotatedPackage(received.data));
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(gfxEntityRotatedPackage);
    }
}

void Server::handleMessage(const RemoveGfxEntityMessage& received)
{
    size_t id;

    std::tie(id) = received.data;

    std::shared_ptr<BasePackage> gfxEntityRemovedPackage(new GfxEntityRemovedPackage(received.data));
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(gfxEntityRemovedPackage);
    }

    graphicsEntities.erase(id); //temphack
}

void Server::handleMessage(const PlayerConnectedToEntityMessage& received)
{
    std::shared_ptr<BasePackage> playerConnectedToEntityPackage(new PlayerConnectedToEntityPackage(received.data));
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(playerConnectedToEntityPackage);
    }
}

void Server::handleMessage(const PlayerFacingBlockMessage& received)
{
    size_t id;
    glm::vec3 vector;
    float x;
    float y;
    float z;

    std::tie(id, vector) = received.data;

    x = vector.x;
    y = vector.y;
    z = vector.z;

    std::shared_ptr<BasePackage> playerFacingBlockPackage(new PlayerFacingBlockPackage(id, x, y, z));
    mClients.at(id)->enqueuePackage(playerFacingBlockPackage);

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

    std::shared_ptr<BasePackage> playerIdPackage(new PlayerIdPackage(newClientId));
    client->enqueuePackage(playerIdPackage);

    //resend current gfx entities. this is a hack right now. in the futuer it probably has to send the whole game state or something, i dunno
    for(size_t id : graphicsEntities)
    {
        std::shared_ptr<BasePackage> gfxEntityAddedPackage(new GfxEntityAddedPackage(id, glm::vec3(0.0f, 0.0f, 0.0f)));
        client->enqueuePackage(gfxEntityAddedPackage);
    }

    //resend chunk created messages
    for(const auto& chunk : mWorld.getWorldInterface().getChunkList())
    {
        std::shared_ptr<BasePackage> chunkAddedPackage(new ChunkLoadedPackage(chunk.second.getLocation(), chunk.second.getVoxelTypes()));
        client->enqueuePackage(chunkAddedPackage);
    }

    mBus.sendMessage<PlayerJoinedMessage>(PlayerJoinedMessage(newClientId, glm::vec3(0.0f, 50.0f, -50.0f))); //position could be loaded from file or at spawn
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
            else if(package->mType == typeid(PlayerActionPackage))
            {
                PlayerActionPackage* playerActionPackage = (PlayerActionPackage*) package.get();
                mBus.sendMessage<PlayerActionMessage>(PlayerActionMessage(playerActionPackage->getData()));
            }
            else if(package->mType == typeid(PlayerPitchYawPackage))
            {
                PlayerPitchYawPackage* playerPitchYawPackage = (PlayerPitchYawPackage*) package.get();
                mBus.sendMessage<PlayerPitchYawMessage>(PlayerPitchYawMessage(playerPitchYawPackage->getData()));
            }
        }
    }
}
