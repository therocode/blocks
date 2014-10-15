#include "servernetworkingsystem.hpp"
#include "../world/chunk.hpp"
#include "../lognames.hpp"
#include "../script/scriptmessages.hpp"
#include "../input/inputmessages.hpp"
#include "channels.hpp"
#include "messageserializer.hpp"
#include "../gameinterface.hpp"
#include "../world/worldsystem.hpp"

ServerNetworkingSystem::ServerNetworkingSystem(fea::MessageBus& bus, const GameInterface& gameInterface, const NetworkParameters& parameters) :
    mBus(bus),
    mGameInterface(gameInterface),
    mParameters(parameters),
    mAcceptingClients(false),
    mMaxPlayerAmount(20),
    mPlayerAmount(0),
    mNextClientId(0),
    mLocalPlayerId(-1),
    mLocalClientBus(nullptr)
{

    subscribe(mBus, *this);

    if(parameters.mode == NetworkMode::SINGLE_PLAYER)
    {
        mBus.send(LogMessage{"Setting up single player networking", serverName, LogLevel::INFO});
    }
    else if(parameters.mode == NetworkMode::DEDICATED)
    {
        mENet = std::unique_ptr<ENet>(new ENet());

        if(mENet->isInitialized())
        {
            mBus.send(LogMessage{"Setting up dedicated server networking", serverName, LogLevel::INFO});

            mENetServer = std::unique_ptr<ENetServer>(new ENetServer(*mENet, parameters.port, CHANNEL_AMOUNT));
            mENetServer->setConnectedCallback(std::bind(&ServerNetworkingSystem::acceptRemoteClient, this, std::placeholders::_1));
            mENetServer->setDataReceivedCallback(std::bind(&ServerNetworkingSystem::handleClientData, this, std::placeholders::_1, std::placeholders::_2));
            mENetServer->setDisconnectedCallback(std::bind(&ServerNetworkingSystem::disconnectRemoteClient, this, std::placeholders::_1));

            if(mENetServer->isListening())
                mBus.send(LogMessage{"Now listening on port " + std::to_string(parameters.port), serverName, LogLevel::INFO});
            else
                mBus.send(LogMessage{"Could not bind to port " + std::to_string(parameters.port), serverName, LogLevel::ERR});
        }
        else
        {
            mBus.send(LogMessage{"Could not initialize networking", serverName, LogLevel::ERR});
        }
    }
    else if(parameters.mode == NetworkMode::COMBINED)
    {
        mENet = std::unique_ptr<ENet>(new ENet());

        if(mENet->isInitialized())
        {
            mBus.send(LogMessage{"Setting up networking", serverName, LogLevel::INFO});

            mENetServer = std::unique_ptr<ENetServer>(new ENetServer(*mENet, parameters.port, CHANNEL_AMOUNT));
            mENetServer->setConnectedCallback(std::bind(&ServerNetworkingSystem::acceptRemoteClient, this, std::placeholders::_1));
            mENetServer->setDataReceivedCallback(std::bind(&ServerNetworkingSystem::handleClientData, this, std::placeholders::_1, std::placeholders::_2));
            mENetServer->setDisconnectedCallback(std::bind(&ServerNetworkingSystem::disconnectRemoteClient, this, std::placeholders::_1));

            if(mENetServer->isListening())
                mBus.send(LogMessage{"Now listening on port " + std::to_string(parameters.port), serverName, LogLevel::INFO});
            else
                mBus.send(LogMessage{"Could not bind to port " + std::to_string(parameters.port), serverName, LogLevel::ERR});
        }
        else
        {
            mBus.send(LogMessage{"Could not initialize networking", serverName, LogLevel::ERR});
        }
    }

    //these should be passed from the outside:
    mSettings.serverName = "Wongaloria";
    mSettings.motd = "Welcome to our glorious server! Please to not grief and take all belonings with you when you leave, thanks.";
    mSettings.maxChunkViewDistance = 8;
}

void ServerNetworkingSystem::handleMessage(const LocalConnectionAttemptMessage& received)
{
    uint32_t newId = mNextClientId++;

    mLocalPlayerId = newId;
    mLocalClientBus = received.clientBus;
    mPlayerPositions.emplace(newId, ChunkCoord(0, 0, 0));
    mPlayerWorlds.emplace(newId, 0);

    mBus.send(LogMessage{"Client connected locally and given player Id " + std::to_string(newId), serverName, LogLevel::INFO});

    mLocalClientBus->send(LocalConnectionEstablishedMessage{&mBus});
}

void ServerNetworkingSystem::handleMessage(const LocalDisconnectionMessage& received)
{
    FEA_ASSERT(mLocalClientBus != nullptr, "Local client trying to disconnect despite it not being connected");

    mBus.send(LogMessage{"Local client disconnected", serverName, LogLevel::INFO});
    mBus.send(PlayerLeftGameMessage{mLocalPlayerId});

    mPlayerPositions.erase(mLocalPlayerId);
    mLocalPlayerId = -1;
    mLocalClientBus = nullptr;
}

void ServerNetworkingSystem::handleMessage(const FrameMessage& received)
{
    if(mENetServer)
        mENetServer->update(0);
}

void ServerNetworkingSystem::handleMessage(const GameStartMessage& received)
{
    mAcceptingClients = true;
}

void ServerNetworkingSystem::handleMessage(const ClientJoinRequestedMessage& received)
{
    mBus.send(LogMessage{"Local client requested to join the game with player name " + received.playerName, serverName, LogLevel::INFO});

    if(mPlayerAmount >= mMaxPlayerAmount)
    {
        ClientJoinDeniedMessage message{JoinDenyReason::FULL, 20, 20};
        mLocalClientBus->send(message);
    }
    else
    {
        ClientJoinAcceptedMessage message{mSettings};
        mLocalClientBus->send(message);

        mBus.send(LogMessage{"Accepting local client's desire to join the game. Sending server settings", serverName, LogLevel::INFO});
        mBus.send(PlayerJoinedGameMessage{mLocalPlayerId});
    }
}

void ServerNetworkingSystem::handleMessage(const ClientRequestedChunksMessage& received)
{
    if(mLocalClientBus != nullptr)
    {
        playerRequestedChunks(mLocalPlayerId, received.worldIdentifier, received.coordinates);
    }
}

void ServerNetworkingSystem::handleMessage(const PlayerEntersChunkMessage& received)
{
    std::cout << "player moved to " << received.chunkCoord.x << " " << received.chunkCoord.y << " " << received.chunkCoord.z << "\n";
    mPlayerPositions.at(received.playerId) = received.chunkCoord;
}

void ServerNetworkingSystem::handleMessage(const PlayerEntersWorldMessage& received)
{
    mPlayerWorlds.at(received.playerId) = received.worldId;
}

void ServerNetworkingSystem::handleMessage(const ChunksDataDeniedMessage& received)
{
    if(received.coordinates.size() > 0)
    {
       for(const auto& chunk : received.coordinates)
       {
           //get 
       }
    }
}

void ServerNetworkingSystem::handleMessage(const ChunksDataDeliveredMessage& received)
{
    //if(received.chunks.size() > 0)
    //{
    //    ClientChunksDeliverMessage message;

    //    for(const auto& chunkIterator : received.chunks)
    //    {
    //        message.coordinates.push_back(chunkIterator.first);

    //        VoxelTypeData voxelData = chunkIterator.second.getVoxelTypeData();
    //        message.rleData.push_back({voxelData.mRleSegmentIndices, voxelData.mRleSegments});
    //    }
    //}
}

void ServerNetworkingSystem::handleMessage(const ChunkFinishedMessage& received)
{
}

void ServerNetworkingSystem::acceptRemoteClient(uint32_t id)
{
    if(mAcceptingClients)
    {
        mBus.send(LogMessage{"Client Id " + std::to_string(id) + " connected", serverName, LogLevel::INFO});
    }
    else
    {
        mBus.send(LogMessage{"Player connected prematurely, disconnecting player", serverName, LogLevel::WARN});
        mENetServer->disconnectOne(id, 500);
    }
}

void ServerNetworkingSystem::handleClientData(uint32_t clientId, const std::vector<uint8_t>& data)
{
    try
    {
        int32_t type = decodeType(data);

        if(type == CLIENT_JOIN_REQUESTED)
        {
            ClientJoinRequestedMessage received = deserializeMessage<ClientJoinRequestedMessage>(data);
            mBus.send(LogMessage{"Client id " + std::to_string(clientId) + " requested to join the game with player name " + received.playerName, serverName, LogLevel::INFO});

            if(mPlayerAmount >= mMaxPlayerAmount)
            {
                ClientJoinDeniedMessage message{JoinDenyReason::FULL, mPlayerAmount, mMaxPlayerAmount};
                mENetServer->sendToOne(clientId, serializeMessage(message), true, CHANNEL_DEFAULT);
            }
            else
            {
                ClientJoinAcceptedMessage message{mSettings};
                mENetServer->sendToOne(clientId, serializeMessage(message), true, CHANNEL_DEFAULT);

                uint32_t newId = mNextClientId++;
                mClientToPlayerIds.emplace(clientId, newId);
                mPlayerToClientIds.emplace(newId, clientId);
                mPlayerPositions.emplace(newId, ChunkCoord(0, 0, 0));
                mPlayerWorlds.emplace(newId, 0);

                mBus.send(LogMessage{"Accepting client " + std::to_string(clientId) + "'s desire to join the game and assigning player Id " + std::to_string(newId) + ". Sending server settings", serverName, LogLevel::INFO});

                mBus.send(PlayerJoinedGameMessage{mClientToPlayerIds.at(clientId)});
            }
        }
        else if(type == CLIENT_REQUESTED_CHUNKS)
        {
            if(mClientToPlayerIds.count(clientId) != 0)
            {
            ClientRequestedChunksMessage received = deserializeMessage<ClientRequestedChunksMessage>(data);

            if(mClientToPlayerIds.count(clientId) != 0)
                playerRequestedChunks(mClientToPlayerIds.at(clientId), received.worldIdentifier, received.coordinates);
            }
            else
            { //client violated protocol by requesting chunks without being part of the game
                mENetServer->disconnectOne(clientId, 200);
            }
        }
        else if(type == TEST_1)
            mBus.send(LogMessage{"Received meaningless test message", serverName, LogLevel::WARN});
        else if(type == TEST_2)
            mBus.send(LogMessage{"Received meaningless test message", serverName, LogLevel::WARN});
        else if(type == INVALID)
            mBus.send(LogMessage{"Received invalid message", serverName, LogLevel::WARN});
        else
            mBus.send(LogMessage{"Received message of unknown type: " + std::to_string(type), serverName, LogLevel::WARN});
    } 
    catch (const DeserializeException& e)
    {
        mBus.send(LogMessage{"Received corrupt/unserializable message", serverName, LogLevel::WARN});
    }
}

void ServerNetworkingSystem::disconnectRemoteClient(uint32_t id)
{
    //if the id doesn't exist in the map, this is a client which has not been allowed into the game disconnecting
    if(mClientToPlayerIds.count(id) != 0)
    {
        uint32_t playerId = mClientToPlayerIds.at(id);

        mBus.send(LogMessage{"Client Id " + std::to_string(id) + ", player Id " + std::to_string(playerId) + " disconnected", serverName, LogLevel::INFO});

        mClientToPlayerIds.erase(id);
        mPlayerToClientIds.erase(playerId);
        mPlayerPositions.erase(playerId);
        mPlayerWorlds.erase(playerId);

        mBus.send(PlayerLeftGameMessage{playerId});
    }
    else
    {
        mBus.send(LogMessage{"Client Id " + std::to_string(id) + " disconnected", serverName, LogLevel::INFO});
    }
}

void ServerNetworkingSystem::playerRequestedChunks(uint32_t id, const std::string& worldIdentifier, const std::vector<ChunkCoord>& chunks)
{
    std::vector<ChunkCoord> inRange;
    std::vector<ChunkCoord> notInRange;

    inRange.reserve(chunks.size() / 2);
    notInRange.reserve(chunks.size() / 2);

    const ChunkCoord& playerChunk = mPlayerPositions.at(id);

    std::cout << "player is in chunk " << playerChunk.x << " " << playerChunk.y << "" << playerChunk.z << "\n";

    for(const auto& chunk : chunks)
    {
        if(glm::distance(glm::vec3(chunk), glm::vec3(playerChunk)) <= (float)mSettings.maxChunkViewDistance)
            inRange.push_back(chunk);
        else
            notInRange.push_back(chunk);
    }

    if(notInRange.size() > 0)
    {
        sendToOne(id, ClientChunksDeniedMessage{worldIdentifier, notInRange}, true, CHANNEL_CHUNKS);
    }
    if(inRange.size() > 0)
    {
        auto& chunkRequestHandler = mChunkRequestHandlers[mGameInterface.getWorldSystem().worldIdentifierToId(worldIdentifier)];
        for(const auto& chunk : inRange)
        {
            chunkRequestHandler.addRequest(id, chunk);
        }

        mBus.send(ChunksRequestedMessage{mPlayerWorlds.at(id), inRange});
    }
}
