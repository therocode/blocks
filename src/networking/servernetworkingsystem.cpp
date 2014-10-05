#include "servernetworkingsystem.hpp"
#include "../world/chunk.hpp"
#include "../lognames.hpp"
#include "../script/scriptmessages.hpp"
#include "../input/inputmessages.hpp"
#include "networkingprotocol.hpp"
#include "messageserializer.hpp"

ServerNetworkingSystem::ServerNetworkingSystem(fea::MessageBus& bus, const NetworkParameters& parameters) :
    mBus(bus),
    mParameters(parameters),
    mAcceptingClients(false),
    mNextClientId(0),
    mLocalPlayerId(0),
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

            mENetServer = std::unique_ptr<ENetServer>(new ENetServer(*mENet, parameters.port));
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

            mENetServer = std::unique_ptr<ENetServer>(new ENetServer(*mENet, parameters.port));
            mENetServer = std::unique_ptr<ENetServer>(new ENetServer(*mENet, parameters.port));
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
}

void ServerNetworkingSystem::handleMessage(const LocalConnectionAttemptMessage& received)
{
    uint32_t newId = mNextClientId++;

    mLocalPlayerId = newId;
    mLocalClientBus = received.clientBus;

    //mBus.send(LogMessage{"Client connected locally and given player Id " + std::to_string(newId), serverName, LogLevel::INFO});
    //mBus.send(PlayerJoinedMessage{newId, 0, {0.0f, 0.0f, 0.0f}});

    mBus.send(LocalConnectionEstablishedMessage{&mBus});
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

void ServerNetworkingSystem::acceptRemoteClient(uint32_t id)
{
    uint32_t newId = mNextClientId++;

    if(mAcceptingClients)
    {
        mClientToPlayerIds.emplace(id, newId);
        mPlayerToClientIds.emplace(newId, id);

        //mBus.send(LogMessage{"Client Id " + std::to_string(id) + " connected and given player Id " + std::to_string(newId), serverName, LogLevel::INFO});
        //mBus.send(PlayerJoinedMessage{newId, 0, {0.0f, 0.0f, 0.0f}});
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

    if(type == CLIENT_JOIN_REQUESTED)
    {
        ClientJoinRequestedMessage message = deserializeMessage<ClientJoinRequestedMessage>(data);
        mBus.send(LogMessage{"Client id " + std::to_string(clientId) + " requested to join the game with player name " + message.playerName, serverName, LogLevel::INFO});
    }
    else if(type == TEST_1)
        mBus.send(LogMessage{"Received meaningless test message", serverName, LogLevel::WARN});
    else if(type == TEST_2)
        mBus.send(LogMessage{"Received meaningless test message", serverName, LogLevel::WARN});
    else if(type == INVALID)
        mBus.send(LogMessage{"Received invalid message", serverName, LogLevel::WARN});
    else
        mBus.send(LogMessage{"Received message of unknown type", serverName, LogLevel::WARN});
}

void ServerNetworkingSystem::disconnectRemoteClient(uint32_t id)
{
    //if the id doesn't exist in the map, this is a client which has not been allowed into the game disconnecting
    if(mClientToPlayerIds.count(id) != 0)
    {
        uint32_t playerId = mClientToPlayerIds.at(id);

        mBus.send(LogMessage{"Client Id " + std::to_string(id) + ", player Id " + std::to_string(playerId) + " disconnected", serverName, LogLevel::INFO});
        //mBus.send(PlayerDisconnectedMessage{playerId});

        mClientToPlayerIds.erase(id);
        mPlayerToClientIds.erase(playerId);
    }
    else
    {
        mBus.send(LogMessage{"Client Id " + std::to_string(id) + " disconnected", serverName, LogLevel::INFO});
    }
}
