#include "server.h"
#include "../networking/packages.h"

Server::Server() : mUniverse(mBus),
    mLogger(mBus, LogLevel::VERB),
    mScriptHandler(mBus, mUniverse.getWorldInterface()),
    mLogName("server")
{
    mBus.addSubscriber<FatalMessage>(*this);
    mBus.addSubscriber<AddGfxEntityMessage>(*this);
    mBus.addSubscriber<MoveGfxEntityMessage>(*this);
    mBus.addSubscriber<RotateGfxEntityMessage>(*this);
    mBus.addSubscriber<RemoveGfxEntityMessage>(*this);
    mBus.addSubscriber<PlayerConnectedToEntityMessage>(*this);
    mBus.addSubscriber<PlayerFacingBlockMessage>(*this);
    mBus.addSubscriber<ChunkDeliverMessage>(*this);
    mBus.addSubscriber<ChunkDeletedMessage>(*this);
    mBus.addSubscriber<VoxelSetMessage>(*this);
}

Server::~Server()
{
    mBus.removeSubscriber<FatalMessage>(*this);
    mBus.removeSubscriber<AddGfxEntityMessage>(*this);
    mBus.removeSubscriber<MoveGfxEntityMessage>(*this);
    mBus.removeSubscriber<RotateGfxEntityMessage>(*this);
    mBus.removeSubscriber<RemoveGfxEntityMessage>(*this);
    mBus.removeSubscriber<PlayerConnectedToEntityMessage>(*this);
    mBus.removeSubscriber<PlayerFacingBlockMessage>(*this);
    mBus.removeSubscriber<ChunkDeliverMessage>(*this);
    mBus.removeSubscriber<ChunkDeletedMessage>(*this);
    mBus.removeSubscriber<VoxelSetMessage>(*this);
}

void Server::setup()
{
    mScriptHandler.setup();
    mUniverse.setup();
    mFPSController.setMaxFPS(60);
    mBus.send<LogMessage>(LogMessage("Server initialised and ready to go", mLogName, LogLevel::INFO));
    mBus.send<GameStartMessage>(GameStartMessage());
}
fea::MessageBus& Server::getBus()
{
    return mBus;
}
void Server::doLogic()
{
    //mFPSController.frameBegin();
    checkForDisconnectedClients();

    for(auto& client : mClients)
    {
        fetchClientData(client.second);
    }

    mBus.send<FrameMessage>(FrameMessage(true));

    mUniverse.update();

    for(auto& client : mClients)
    {
        client.second->flush();
    }

    pollNewClients();

    mFPSController.frameEnd();
}

void Server::destroy()
{
    mUniverse.destroy();
    mScriptHandler.destroy();
    mBus.send<LogMessage>(LogMessage("Server destroyed", mLogName, LogLevel::INFO));
}

void Server::handleMessage(const FatalMessage& received)
{
    std::string message;

    std::tie(message) = received.mData;
    mBus.send<LogMessage>(LogMessage(message, mLogName, LogLevel::ERR));
    exit(4);
}

void Server::handleMessage(const ChunkDeliverMessage& received)
{
    ChunkCoord coordinate;
    Chunk chunk;

    std::tie(coordinate, chunk) = received.mData;

    VoxelTypeData typeData = chunk.getVoxelTypeData();

    std::shared_ptr<BasePackage> chunkLoadedPackage(new ChunkLoadedPackage(coordinate, typeData.mRleSegmentIndices, typeData.mRleSegments));

    for(auto& client : mClients)
    {
        client.second->enqueuePackage(chunkLoadedPackage);
    }
}

void Server::handleMessage(const ChunkDeletedMessage& received)
{
    std::shared_ptr<BasePackage> chunkDeletedPackage(new ChunkDeletedPackage(received.mData));
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(chunkDeletedPackage);
    }
}

void Server::handleMessage(const AddGfxEntityMessage& received)
{
    size_t id;

    std::tie(id, std::ignore) = received.mData;

    std::shared_ptr<BasePackage> gfxEntityAddedPackage(new GfxEntityAddedPackage(received.mData));
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(gfxEntityAddedPackage);
    }

    graphicsEntities.insert(id); //temphack
}

void Server::handleMessage(const MoveGfxEntityMessage& received)
{
    std::shared_ptr<BasePackage> gfxEntityMovedPackage(new GfxEntityMovedPackage(received.mData));
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(gfxEntityMovedPackage);
    }
}

void Server::handleMessage(const RotateGfxEntityMessage& received)
{
    std::shared_ptr<BasePackage> gfxEntityRotatedPackage(new GfxEntityRotatedPackage(received.mData));
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(gfxEntityRotatedPackage);
    }
}

void Server::handleMessage(const RemoveGfxEntityMessage& received)
{
    size_t id;

    std::tie(id) = received.mData;

    std::shared_ptr<BasePackage> gfxEntityRemovedPackage(new GfxEntityRemovedPackage(received.mData));
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(gfxEntityRemovedPackage);
    }

    graphicsEntities.erase(id); //temphack
}

void Server::handleMessage(const PlayerConnectedToEntityMessage& received)
{
    std::shared_ptr<BasePackage> playerConnectedToEntityPackage(new PlayerConnectedToEntityPackage(received.mData));
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(playerConnectedToEntityPackage);
    }
}

void Server::handleMessage(const PlayerFacingBlockMessage& received)
{
    size_t id;
    VoxelCoord vector;
    int x;
    int y;
    int z;

    std::tie(id, vector) = received.mData;

    x = vector.x;
    y = vector.y;
    z = vector.z;

    std::shared_ptr<BasePackage> playerFacingBlockPackage(new PlayerFacingBlockPackage(id, x, y, z));
    mClients.at(id)->enqueuePackage(playerFacingBlockPackage);

}

void Server::handleMessage(const VoxelSetMessage& received)
{
    std::shared_ptr<BasePackage> voxelSetPackage(new VoxelSetPackage(received.mData));
    for(auto& client : mClients)
    {
        client.second->enqueuePackage(voxelSetPackage);
    }
}

void Server::setClientListener(std::unique_ptr<ClientConnectionListener> clientListener)
{
    mListener = std::move(clientListener);
}

void Server::acceptClientConnection(std::shared_ptr<ClientConnection> client)
{
    ClientId newClientId = client->getId();

    mClients.emplace(newClientId, client);

    mBus.send<LogMessage>(LogMessage(std::string("Client id ") + std::to_string(newClientId) + std::string(" connected"), mLogName, LogLevel::INFO));

    std::shared_ptr<BasePackage> playerIdPackage(new PlayerIdPackage(newClientId));
    client->enqueuePackage(playerIdPackage);

    //resend current gfx entities. this is a hack right now. in the futuer it probably has to send the whole game state or something, i dunno
    for(size_t id : graphicsEntities)
    {
        std::shared_ptr<BasePackage> gfxEntityAddedPackage(new GfxEntityAddedPackage(id, glm::vec3(0.0f, 0.0f, 0.0f)));
        client->enqueuePackage(gfxEntityAddedPackage);
    }

    //resend chunk created messages
    for(const auto& chunk : mUniverse.getWorldInterface().getChunkMap())
    {
        std::shared_ptr<BasePackage> chunkAddedPackage(new ChunkLoadedPackage(chunk.first, chunk.second.getVoxelTypeData().mRleSegmentIndices, chunk.second.getVoxelTypeData().mRleSegments));
        client->enqueuePackage(chunkAddedPackage);
    }

    mBus.send<PlayerJoinedMessage>(PlayerJoinedMessage(newClientId, glm::vec3(0.0f, 45.0f, 0.0f))); //position could be loaded from file or at spawn
}

void Server::pollNewClients()
{
    std::shared_ptr<ClientConnection> client;

    if(mListener != nullptr)
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
            if(package->mType == PackageType::REBUILD_SCRIPTS_REQUESTED)
            {
                mBus.send<RebuildScriptsRequestedMessage>(RebuildScriptsRequestedMessage('0'));
            }
            else if(package->mType == PackageType::PLAYER_ACTION)
            {
                PlayerActionPackage* playerActionPackage = (PlayerActionPackage*) package.get();
                mBus.send<PlayerActionMessage>(PlayerActionMessage(playerActionPackage->getData()));
            }
            else if(package->mType == PackageType::PLAYER_MOVE_DIRECTION)
            {
                PlayerMoveDirectionPackage* playerMoveDirectionPackage = (PlayerMoveDirectionPackage*) package.get();
                size_t id;
                int8_t forwardsBack;
                int8_t leftRight;
                std::tie(id, forwardsBack, leftRight) = playerMoveDirectionPackage->getData();

                MoveDirection dir(forwardsBack, leftRight);

                mBus.send<PlayerMoveDirectionMessage>(PlayerMoveDirectionMessage(id, dir));
            }
            else if(package->mType == PackageType::PLAYER_MOVE_ACTION)
            {
                PlayerMoveActionPackage* playerMoveActionPackage = (PlayerMoveActionPackage*) package.get();

                mBus.send<PlayerMoveActionMessage>(PlayerMoveActionMessage(playerMoveActionPackage->getData()));
            }
            else if(package->mType == PackageType::PLAYER_PITCH_YAW)
            {
                PlayerPitchYawPackage* playerPitchYawPackage = (PlayerPitchYawPackage*) package.get();
                mBus.send<PlayerPitchYawMessage>(PlayerPitchYawMessage(playerPitchYawPackage->getData()));
            }
        }
    }
}

void Server::checkForDisconnectedClients()
{
    std::vector<size_t> clientsToRemove;

    for(auto& client : mClients)
    {
        if(client.second->isDisconnected())
        {
            clientsToRemove.push_back(client.first);
        }
    }

    for(auto client : clientsToRemove)
    {
        mClients.erase(client);
        //send playerdisconnectedmessage
        mBus.send<PlayerDisconnectedMessage>(PlayerDisconnectedMessage(client));
    }
}
