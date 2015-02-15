#include "clientnetworkingsystem.hpp"
#include "networkingmessages.hpp"
#include "../world/worldmessages.hpp"
#include "../client/clientmessages.hpp"
#include "../lognames.hpp"
#include "enetclient.hpp"
#include "channels.hpp"

ClientNetworkingSystem::ClientNetworkingSystem(fea::MessageBus& bus, const NetworkParameters& parameters) :
    mBus(bus),
    mParameters(parameters),
    mServerBus(nullptr),
    mIsConnected(false)
{
    subscribe(mBus, *this);

    if(parameters.mode == NetworkMode::SINGLE_PLAYER)
    {
        mBus.send(LogMessage{"Setting up single player client", gClientName, LogLevel::INFO});
        mBus.send(LocalConnectionAttemptMessage{&bus});
    }
    else if(parameters.mode == NetworkMode::JOIN)
    {
        mENet = std::unique_ptr<ENet>(new ENet());

        if(mENet->isInitialized())
        {
            mBus.send(LogMessage{"Setting up client networking", gClientName, LogLevel::INFO});

            mENetClient = std::unique_ptr<ENetClient>(new ENetClient(*mENet, CHANNEL_AMOUNT));
            mENetClient->setConnectedCallback(std::bind(&ClientNetworkingSystem::connectedToServer, this));
            mENetClient->setDataReceivedCallback(std::bind(&ClientNetworkingSystem::handleServerData, this, std::placeholders::_1));
            mENetClient->setDisconnectedCallback(std::bind(&ClientNetworkingSystem::disconnectedFromServer, this));

            mENetClient->connect(parameters.address, parameters.port, 5000);
        }
        else
        {
            mBus.send(LogMessage{"Could not initialize networking", gClientName, LogLevel::ERR});
        }
    }
    else if(parameters.mode == NetworkMode::COMBINED)
    {
        mBus.send(LogMessage{"Setting up client networking", gClientName, LogLevel::INFO});
        mBus.send(LocalConnectionAttemptMessage{&bus});

        mENetClient = std::unique_ptr<ENetClient>(new ENetClient(*mENet, CHANNEL_AMOUNT));
    }
}

ClientNetworkingSystem::~ClientNetworkingSystem()
{
    if(mServerBus)
    {
        mBus.send(LogMessage{"Disconnecting locally from server", gClientName, LogLevel::INFO});
        mServerBus->send(LocalDisconnectionMessage{});
    }
}

void ClientNetworkingSystem::handleMessage(const FrameMessage& received)
{
    if(mServerBus != nullptr && received.frameNumber == 5)
    {
        mBus.send(GameStartMessage{});
    }

    if(mIsConnected)
    {
        for(const auto worldIter : mChunksToRequest)
        {
            send(ClientRequestedChunksMessage{mWorldIds.valueFromId(worldIter.first), worldIter.second}, true, CHANNEL_CHUNKS);
        }

        mChunksToRequest.clear();
    }

    if(mENetClient)
        mENetClient->update(0);
}

void ClientNetworkingSystem::handleMessage(const LocalConnectionEstablishedMessage& received)
{
    mServerBus = received.serverBus;
    mIsConnected = true;
    mBus.send(LogMessage{"Connected locally to server", gClientName, LogLevel::INFO});

    mBus.send(LogMessage{"Requesting to join game as '" + std::string("Tobbe") + "'", gClientName, LogLevel::INFO});
    ClientJoinRequestedMessage message{"Tobbe"};
    send(message, true, CHANNEL_DEFAULT);
}

void ClientNetworkingSystem::handleMessage(const ClientJoinDeniedMessage& received)
{
    if(received.reason == JoinDenyReason::FULL)
        mBus.send(LogMessage{"Could not join game at server. Server is full with " + std::to_string(received.playerAmount) + "/" + std::to_string(received.maximumAllowed) + " players", gClientName, LogLevel::INFO});

    if(mENetClient)
        mENetClient->disconnect(400);
}

void ClientNetworkingSystem::handleMessage(const ClientJoinAcceptedMessage& received)
{
    mBus.send(LogMessage{"Successfully joined the game on server " + received.settings.serverName + "! \nMOTD: " + received.settings.motd, gClientName, LogLevel::INFO});
    mBus.send(GameStartMessage{});

    if(mIsConnected)
        send(SubscriptionRequestedMessage{chunkWidth}, true, CHANNEL_DEFAULT);
}

void ClientNetworkingSystem::handleMessage(const ChunksRequestedMessage& received)
{
    mChunksToRequest[received.worldId].insert(mChunksToRequest[received.worldId].end(), received.coordinates.begin(), received.coordinates.end());
}

void ClientNetworkingSystem::handleMessage(const ClientChunksDeliveredMessage& received)
{
    std::unordered_map<ChunkCoord, Chunk> chunks;

    for(int32_t i = 0; i < received.coordinates.size(); i++)
    {
        Chunk chunk(received.rleIndices[i], received.rleSegments[i]);
        chunks.emplace(received.coordinates[i], std::move(chunk));
    }

    mBus.send(ChunksDataDeliveredMessage{mWorldIds.getId(received.worldIdentifier), chunks});
}

void ClientNetworkingSystem::handleMessage(const ClientChunksDeniedMessage& received)
{
    //std::cout << "got " << received.coordinates.size() << " chunks denied!\n";
}

void ClientNetworkingSystem::handleMessage(const SubscriptionReplyMessage& received)
{
    mBus.send(LogMessage{received.granted ? "Server granted subscription" : "Server did not grant subscription request", gClientName, LogLevel::INFO});
}

void ClientNetworkingSystem::handleMessage(const ClientActionMessage& received)
{
    if(received.action != QUIT)
    {
        if(mIsConnected)
            send(received, true, CHANNEL_DEFAULT);
    }
}

void ClientNetworkingSystem::handleMessage(const ClientMoveActionMessage& received)
{
    if(mIsConnected)
        send(received, true, CHANNEL_DEFAULT);
}

void ClientNetworkingSystem::handleMessage(const ClientMoveDirectionMessage& received)
{
    if(mIsConnected)
        send(received, true, CHANNEL_DEFAULT);
}

void ClientNetworkingSystem::handleMessage(const ClientPitchYawMessage& received)
{
    if(mIsConnected)
        send(received, true, CHANNEL_DEFAULT);
}

void ClientNetworkingSystem::handleMessage(const EntityEnteredRangeMessage& received)
{
    size_t returnedId;

    mBus.send(AddGfxEntityMessage{received.position, returnedId});
    mGraphicEntityIds.emplace(received.id, returnedId);

    if(received.id == mPlayerEntity)
    {
        mBus.send(AttachedToGfxEntityMessage{mGraphicEntityIds.at(mPlayerEntity)});
    }
}

void ClientNetworkingSystem::handleMessage(const EntityPositionUpdatedMessage& received)
{
    mBus.send(ClientEntityMovedMessage{received.id, received.position});
    mBus.send(MoveGfxEntityMessage{mGraphicEntityIds.at(received.id), received.position});
}

void ClientNetworkingSystem::handleMessage(const EntityOrientationUpdatedMessage& received)
{
    mBus.send(OrientateGfxEntityMessage{mGraphicEntityIds.at(received.id), received.orientation});

    if(received.id == mPlayerEntity)
        mBus.send(OrientateLocalEntityMessage{received.orientation});
}

void ClientNetworkingSystem::handleMessage(const EntityLeftRangeMessage& received)
{
    mBus.send(RemoveGfxEntityMessage{mGraphicEntityIds.at(received.id)});
}

void ClientNetworkingSystem::handleMessage(const ClientAttachedToEntityMessage& received)
{
    mBus.send(LocalPlayerAttachedToEntityMessage{received.entityId, mWorldIds.getId(received.worldId), received.position, received.highlightRange});
    mPlayerEntity = received.entityId;
}

void ClientNetworkingSystem::handleMessage(const VoxelUpdatedMessage& received)
{
    mBus.send(VoxelSetMessage{received.id, received.voxel, received.type});
}

void ClientNetworkingSystem::connectedToServer()
{
    mBus.send(LogMessage{"Successfully connected to server", gClientName, LogLevel::INFO});

    mBus.send(LogMessage{"Requesting to join game as '" + std::string("Tobbe") + "'", gClientName, LogLevel::INFO});
    ClientJoinRequestedMessage message{"Tobbe"};
    send(message, true, CHANNEL_DEFAULT);
    mIsConnected = true;
}

void ClientNetworkingSystem::handleServerData(const std::vector<uint8_t>& data)
{
    int32_t type = decodeType(data);

    try
    {
        if(type == CLIENT_JOIN_DENIED)
        {
            ClientJoinDeniedMessage received = deserializeMessage<ClientJoinDeniedMessage>(data);
            mBus.send(received);
        }
        else if(type == CLIENT_JOIN_ACCEPTED)
        {
            ClientJoinAcceptedMessage received = deserializeMessage<ClientJoinAcceptedMessage>(data);
            mBus.send(received);
        }
        else if(type == CLIENT_CHUNKS_DENIED)
        {
            ClientChunksDeniedMessage received = deserializeMessage<ClientChunksDeniedMessage>(data);
            mBus.send(received);
        }
        else if(type == SUBSCRIPTION_REPLY)
        {
            SubscriptionReplyMessage received = deserializeMessage<SubscriptionReplyMessage>(data);
            mBus.send(received);
        }
        else if(type == CLIENT_CHUNKS_DELIVERY)
        {
            ClientChunksDeliveredMessage received = deserializeMessage<ClientChunksDeliveredMessage>(data);
            handleMessage(received);
        }
        else if(type == VOXEL_UPDATED)
        {
            VoxelUpdatedMessage received = deserializeMessage<VoxelUpdatedMessage>(data);
            handleMessage(received);
        }
        else if(type == ENTITY_ENTERED_RANGE)
        {
            EntityEnteredRangeMessage received = deserializeMessage<EntityEnteredRangeMessage>(data);
            mBus.send(received);
        }
        else if(type == ENTITY_POSITION_UPDATED)
        {
            EntityPositionUpdatedMessage received = deserializeMessage<EntityPositionUpdatedMessage>(data);
            handleMessage(received);
        }
        else if(type == ENTITY_ORIENTATION_UPDATED)
        {
            EntityOrientationUpdatedMessage received = deserializeMessage<EntityOrientationUpdatedMessage>(data);
            mBus.send(received);
        }
        else if(type == ENTITY_LEFT_RANGE)
        {
            EntityLeftRangeMessage received = deserializeMessage<EntityLeftRangeMessage>(data);
            mBus.send(received);
        }
        else if(type == CLIENT_ATTACHED_TO_ENTITY)
        {
            ClientAttachedToEntityMessage received = deserializeMessage<ClientAttachedToEntityMessage>(data);
            mBus.send(received);
        }
        else if(type == CLIENT_ENTERED_WORLD)
        {
            //ClientEnteredWorldMessage received = deserializeMessage<ClientEnteredWorldMessage>(data);
            //mBus.send(received);
        }
        else if(type == CLIENT_POSITION)
        {
            //ClientPositionMessage received = deserializeMessage<ClientPositionMessage>(data);
            //mBus.send(received);
        }
        else if(type == TEST_1)
            mBus.send(LogMessage{"Received meaningless test message", gClientName, LogLevel::WARN});
        else if(type == TEST_2)
            mBus.send(LogMessage{"Received meaningless test message", gClientName, LogLevel::WARN});
        else if(type == INVALID)
            mBus.send(LogMessage{"Received invalid message", gClientName, LogLevel::WARN});
        else
            mBus.send(LogMessage{"Received message of unknown type", gClientName, LogLevel::WARN});
    } 
    catch(const DeserializeException& e)
    {
        mBus.send(LogMessage{"Received corrupt/unserializable message of type " + std::to_string(type) + "(" + PacketTypeToString(type) + ")", gClientName, LogLevel::WARN});
    }
}

void ClientNetworkingSystem::disconnectedFromServer()
{
    mBus.send(LogMessage{"Disconnected from server", gClientName, LogLevel::INFO});
    mIsConnected = false;
}
