#include "servernetworkingsystem.hpp"
#include "../world/chunk.hpp"
#include "../lognames.hpp"
#include "../script/scriptmessages.hpp"
#include "../input/inputmessages.hpp"
#include "channels.hpp"
#include "messageserializer.hpp"
#include "../gameinterface.hpp"
#include "../world/worldsystem.hpp"
#include "../utilities/distancechecker.hpp"
#include <tuple>

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
    mLocalClientBus = received.clientBus;

    mBus.send(LogMessage{"Client connected locally", serverName, LogLevel::INFO});

    mLocalClientBus->send(LocalConnectionEstablishedMessage{&mBus});
}

void ServerNetworkingSystem::handleMessage(const LocalDisconnectionMessage& received)
{
    FEA_ASSERT(mLocalClientBus != nullptr, "Local client trying to disconnect despite it not being connected");

    mBus.send(LogMessage{"Local client disconnected", serverName, LogLevel::INFO});
    mBus.send(PlayerLeftGameMessage{mLocalPlayerId});

    mPlayerPositions.erase(mLocalPlayerId);
    mSubscriptions.erase(mLocalPlayerId);
    mEntityTracking.erase(mLocalPlayerId);
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
        uint32_t newId = mNextClientId++;
        mLocalPlayerId = newId;
        mPlayerWorlds.emplace(newId, 0);

        mBus.send(LogMessage{"Accepting local client's desire to join the game. Giving player Id " + std::to_string(newId) + ". Sending server settings", serverName, LogLevel::INFO});
        mBus.send(PlayerJoinedGameMessage{mLocalPlayerId});

        ClientJoinAcceptedMessage message{mSettings};
        mLocalClientBus->send(message);
    }
}

void ServerNetworkingSystem::handleMessage(const SubscriptionRequestedMessage& received)
{
    if(mLocalClientBus != nullptr)
    {
        FEA_ASSERT(mLocalPlayerId != -1, "Trying to subscribe before joining!\n");
        mSubscriptions.emplace(mLocalPlayerId, received.chunkDistance);

        mLocalClientBus->send(SubscriptionReplyMessage{true});
    }
}

void ServerNetworkingSystem::handleMessage(const ClientRequestedChunksMessage& received)
{
    if(mLocalClientBus != nullptr)
    {
        playerRequestedChunks(mLocalPlayerId, received.worldIdentifier, received.coordinates);
    }
}

void ServerNetworkingSystem::handleMessage(const ClientActionMessage& received)
{
    if(mLocalClientBus != nullptr)
    {
        mBus.send(PlayerActionMessage{mLocalPlayerId, received.action});
    }
}
        
void ServerNetworkingSystem::handleMessage(const ClientMoveActionMessage& received)
{
    if(mLocalClientBus != nullptr)
    {
        mBus.send(PlayerMoveActionMessage{mLocalPlayerId, received.action});
    }
}

void ServerNetworkingSystem::handleMessage(const ClientMoveDirectionMessage& received)
{
    if(mLocalClientBus != nullptr)
    {
        mBus.send(PlayerMoveDirectionMessage{mLocalPlayerId, received.direction});
    }
}

void ServerNetworkingSystem::handleMessage(const ClientPitchYawMessage& received)
{
    if(mLocalClientBus != nullptr)
    {
        mBus.send(PlayerPitchYawMessage{mLocalPlayerId, received.pitch, received.yaw});
    }
}

void ServerNetworkingSystem::handleMessage(const PlayerAttachedToEntityMessage& received)
{
    mEntityIdToPlayerId.emplace(received.entityId, received.playerId);
    mPlayerPositions[received.playerId] = received.entity.lock()->getAttribute<glm::vec3>("position");
    mPlayerWorlds[received.playerId] = received.entity.lock()->getAttribute<WorldId>("current_world");
    uint32_t highlightRadius = received.entity.lock()->getAttribute<uint32_t>("highlight_radius");

    const std::string worldIdentifier = mGameInterface.getWorldSystem().worldIdToIdentifier(mPlayerWorlds.at(received.playerId));

    //sendToOne(received.playerId, ClientAttachedToEntityMessage{received.entityId, worldIdentifier, mPlayerPositions.at(received.playerId), highlightRadius}, true, CHANNEL_DEFAULT);
    sendToOne(received.playerId, ClientAttachedToEntityMessage{received.entityId, highlightRadius}, true, CHANNEL_DEFAULT);
}

void ServerNetworkingSystem::handleMessage(const PlayerEntityMovedMessage& received)
{
    mPlayerPositions.at(received.playerId) = received.position;

    //sendToOne(received.playerId, ClientPositionMessage{received.position}, false, CHANNEL_DEFAULT);
}

void ServerNetworkingSystem::handleMessage(const PlayerEntersWorldMessage& received)
{
    mPlayerWorlds.at(received.playerId) = received.worldId;

    //sendToOne(received.playerId, ClientEnteredWorldMessage{mGameInterface.getWorldSystem().worldIdToIdentifier(mPlayerWorlds.at(received.playerId))}, true, CHANNEL_DEFAULT);
}

void ServerNetworkingSystem::handleMessage(const EntityCreatedMessage& received)
{
    fea::EntityPtr entity = received.entity.lock();
    WorldId entityWorld = entity->getAttribute<WorldId>("current_world");
    const glm::vec3& entityPosition = entity->getAttribute<glm::vec3>("position");

    for(const auto& subscription : mSubscriptions)
    {
        uint32_t playerId = subscription.first;
        const auto& positionIterator = mPlayerPositions.find(playerId);

        if(positionIterator != mPlayerPositions.end())
        {
            const glm::vec3& playerPosition = positionIterator->second;
            float range = subscription.second * chunkWidth;

            if(glm::distance(entityPosition, playerPosition) < range && entityWorld == mPlayerWorlds.at(playerId))
            {
                mEntityTracking[playerId].emplace(entity->getId());

                EntityEnteredRangeMessage message{entity->getId(), entityPosition};
                sendToOne(playerId, message, true, CHANNEL_DEFAULT);
            }
        }
    }
}

void ServerNetworkingSystem::handleMessage(const EntityMovedMessage& received)
{
    const glm::vec3& entityPosition = received.newPosition;

    for(const auto& subscription : mSubscriptions)
    {
        uint32_t playerId = subscription.first;
        const auto& positionIterator = mPlayerPositions.find(playerId);

        if(positionIterator != mPlayerPositions.end())
        {
            const glm::vec3& playerPosition = positionIterator->second;
            float range = subscription.second * chunkWidth;

            if(glm::distance(entityPosition, playerPosition) < range && received.worldId == mPlayerWorlds.at(playerId))
            {
                auto result = mEntityTracking[playerId].emplace(received.entityId);

                if(result.second)
                {
                    EntityEnteredRangeMessage message{received.entityId, entityPosition};
                    sendToOne(playerId, message, true, CHANNEL_DEFAULT);
                }
                else
                {
                    EntityPositionUpdatedMessage message{received.entityId, entityPosition};
                    sendToOne(playerId, message, false, CHANNEL_DEFAULT);
                }
            }
            else
            {
                auto result = mEntityTracking[playerId].erase(received.entityId);

                if(result > 0)
                {
                    EntityLeftRangeMessage message{received.entityId};
                    sendToOne(playerId, message, true, CHANNEL_DEFAULT);
                }
            }
        }
    }
}

void ServerNetworkingSystem::handleMessage(const EntityRotatedMessage& received)
{
    for(const auto& subscription : mSubscriptions)
    {
        uint32_t playerId = subscription.first;

        if(mEntityTracking[playerId].count(received.id) != 0)
        {
            EntityRotationUpdatedMessage message{received.id, received.pitch, received.yaw};
            sendToOne(playerId, message, true, CHANNEL_DEFAULT);
        }
    }
}

void ServerNetworkingSystem::handleMessage(const EntityRemovedMessage& received)
{
    for(const auto& subscription : mSubscriptions)
    {
        uint32_t playerId = subscription.first;

        auto result = mEntityTracking[playerId].erase(received.id);

        if(result > 0)
        {
            EntityLeftRangeMessage message{received.id};
            sendToOne(playerId, message, true, CHANNEL_DEFAULT);
        }
    }
}

void ServerNetworkingSystem::handleMessage(const ChunksDataDeniedMessage& received)
{
    if(received.coordinates.size() > 0)
    {
        if(mChunkRequestHandlers.count(received.worldId) != 0)
        {
            ChunkRequestHandler& chunkRequestHandler = mChunkRequestHandlers.at(received.worldId);
            std::unordered_map<uint32_t, std::vector<ChunkCoord>> chunksToDeny;

            for(const auto& chunk : received.coordinates)
            {
                auto deniedRequests = chunkRequestHandler.getAndRemove(chunk);

                for(const auto& client : deniedRequests)
                {
                    chunksToDeny[client].push_back(chunk);
                }
            }

            for(const auto iterator : chunksToDeny)
            {
                ClientChunksDeniedMessage message{mGameInterface.getWorldSystem().worldIdToIdentifier(received.worldId), iterator.second};
                sendToOne(iterator.first, message, true, CHANNEL_CHUNKS);
            }
        }
    }
}

void ServerNetworkingSystem::handleMessage(const ChunksDataDeliveredMessage& received)
{
    if(received.chunks.size() > 0)
    {
        if(mChunkRequestHandlers.count(received.worldId) != 0)
        {
            ChunkRequestHandler& chunkRequestHandler = mChunkRequestHandlers.at(received.worldId);
            std::unordered_map<uint32_t, std::tuple<std::vector<ChunkCoord>, std::vector<RleIndexArray>, std::vector<RleSegmentArray>>> chunksToDeliver;

            for(const auto& chunk : received.chunks)
            {
                auto acceptedRequests = chunkRequestHandler.getAndRemove(chunk.first);

                for(const auto& client : acceptedRequests)
                {
                    const auto& voxelRleData = chunk.second.getVoxelTypeData();
                    std::get<0>(chunksToDeliver[client]).push_back(chunk.first);
                    std::get<1>(chunksToDeliver[client]).push_back(voxelRleData.mRleSegmentIndices);
                    std::get<2>(chunksToDeliver[client]).push_back(voxelRleData.mRleSegments);
                }
            }


            for(const auto iterator : chunksToDeliver)
            {
                const auto& tuple = chunksToDeliver[iterator.first];
                ClientChunksDeliveredMessage message{mGameInterface.getWorldSystem().worldIdToIdentifier(received.worldId), std::get<0>(tuple), std::get<1>(tuple), std::get<2>(tuple)};
                sendToOne(iterator.first, message, true, CHANNEL_CHUNKS);
            }
        }
    }
}

void ServerNetworkingSystem::handleMessage(const ChunkFinishedMessage& received)
{
    if(mChunkRequestHandlers.count(received.worldId) != 0)
    {
        ChunkRequestHandler& chunkRequestHandler = mChunkRequestHandlers.at(received.worldId);

        auto acceptedRequests = chunkRequestHandler.getAndRemove(received.coordinate);

        const auto& voxelRleData = received.chunk.getVoxelTypeData();
        ClientChunksDeliveredMessage message{mGameInterface.getWorldSystem().worldIdToIdentifier(received.worldId), {received.coordinate}, {voxelRleData.mRleSegmentIndices}, {voxelRleData.mRleSegments}};

        for(const auto& client : acceptedRequests)
        {
            sendToOne(client, message, true, CHANNEL_CHUNKS);
        }
    }
}

void ServerNetworkingSystem::handleMessage(const VoxelSetMessage& received)
{
    ChunkCoord chunk = VoxelToChunk::convert(received.voxel);

    for(const auto& subscription : mSubscriptions)
    {
        uint32_t playerId = subscription.first;
        const auto& positionIterator = mPlayerPositions.find(playerId);

        if(positionIterator != mPlayerPositions.end())
        {
            if(received.worldId == mPlayerWorlds.at(playerId))
            {
                ChunkCoord playerChunk = WorldToChunk::convert(positionIterator->second);

                if(DistanceChecker::isWithinDistanceOf(chunk, playerChunk, mSettings.maxChunkViewDistance))
                {
                    VoxelUpdatedMessage message{received.voxel, received.type};
                    sendToOne(playerId, message, true, CHANNEL_DEFAULT);
                }
            }
        }
    }
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
    int32_t type = decodeType(data);

    try
    {
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
                playerRequestedChunks(mClientToPlayerIds.at(clientId), received.worldIdentifier, received.coordinates);
            }
            else
            { //client violated protocol by requesting chunks without being part of the game
                mENetServer->disconnectOne(clientId, 200);
            }
        }
        else if(type == CLIENT_ACTION)
        {
            if(mClientToPlayerIds.count(clientId) != 0)
            {
                ClientActionMessage received = deserializeMessage<ClientActionMessage>(data);
                PlayerActionMessage message{mClientToPlayerIds.at(clientId), received.action};
                mBus.send(message);
            }
        }
        else if(type == CLIENT_MOVE_ACTION)
        {
            if(mClientToPlayerIds.count(clientId) != 0)
            {
                ClientMoveActionMessage received = deserializeMessage<ClientMoveActionMessage>(data);
                PlayerMoveActionMessage message{mClientToPlayerIds.at(clientId), received.action};
                mBus.send(message);
            }
        }
        else if(type == CLIENT_MOVE_DIRECTION)
        {
            if(mClientToPlayerIds.count(clientId) != 0)
            {
                ClientMoveDirectionMessage received = deserializeMessage<ClientMoveDirectionMessage>(data);
                PlayerMoveDirectionMessage message{mClientToPlayerIds.at(clientId), received.direction};
                mBus.send(message);
            }
        }
        else if(type == CLIENT_PITCH_YAW)
        {
            if(mClientToPlayerIds.count(clientId) != 0)
            {
                ClientPitchYawMessage received = deserializeMessage<ClientPitchYawMessage>(data);
                PlayerPitchYawMessage message{mClientToPlayerIds.at(clientId), received.pitch, received.yaw};
                mBus.send(message);
            }
        }
        else if(type == SUBSCRIPTION_REQUESTED)
        {
            if(mClientToPlayerIds.count(clientId) != 0)
            {
                SubscriptionRequestedMessage received = deserializeMessage<SubscriptionRequestedMessage>(data);
                mSubscriptions.emplace(mClientToPlayerIds.at(clientId), received.chunkDistance);

                mENetServer->sendToOne(clientId, serializeMessage(SubscriptionReplyMessage{true}), true, CHANNEL_DEFAULT);
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
        mBus.send(LogMessage{"Received corrupt/unserializable message of type " + std::to_string(type) + "(" + PacketTypeToString(type) + ")", serverName, LogLevel::WARN});
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
        mSubscriptions.erase(playerId);
        mEntityTracking.erase(playerId);
        mPlayerWorlds.erase(playerId);
        
        for(auto& requestHandler : mChunkRequestHandlers)
            requestHandler.second.clearRequestsFor(playerId);

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

    const ChunkCoord& playerChunk = WorldToChunk::convert(mPlayerPositions.at(id));

    for(const auto& chunk : chunks)
    {
        if(DistanceChecker::isWithinDistanceOf(chunk, playerChunk, mSettings.maxChunkViewDistance))
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
        if(mGameInterface.getWorldSystem().hasWorld(worldIdentifier))
        {
            auto& chunkRequestHandler = mChunkRequestHandlers[mGameInterface.getWorldSystem().worldIdentifierToId(worldIdentifier)];

            for(const auto& chunk : inRange)
            {
                chunkRequestHandler.addRequest(id, chunk);
            }

            mBus.send(ChunksRequestedMessage{mPlayerWorlds.at(id), inRange});
        }
        else
        {
            mBus.send(LogMessage{"Client Id " + std::to_string(id) + " requested chunks of nonexisting world " + worldIdentifier, serverName, LogLevel::WARN});
        }
    }
}
