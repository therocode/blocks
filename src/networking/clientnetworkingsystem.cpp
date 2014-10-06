#include "clientnetworkingsystem.hpp"
#include "networkingmessages.hpp"
#include "../world/worldmessages.hpp"
#include "../lognames.hpp"
#include "enetclient.hpp"
#include "channels.hpp"

ClientNetworkingSystem::ClientNetworkingSystem(fea::MessageBus& bus, const NetworkParameters& parameters) :
    mBus(bus),
    mParameters(parameters),
    mServerBus(nullptr)
{
    subscribe(mBus, *this);

    if(parameters.mode == NetworkMode::SINGLE_PLAYER)
    {
        mBus.send(LogMessage{"Setting up single player client", netName, LogLevel::INFO});
        mBus.send(LocalConnectionAttemptMessage{&bus});
    }
    else if(parameters.mode == NetworkMode::JOIN)
    {
        mENet = std::unique_ptr<ENet>(new ENet());

        if(mENet->isInitialized())
        {
            mBus.send(LogMessage{"Setting up client networking", netName, LogLevel::INFO});

            mENetClient = std::unique_ptr<ENetClient>(new ENetClient(*mENet, CHANNEL_AMOUNT));
            mENetClient->setConnectedCallback(std::bind(&ClientNetworkingSystem::connectedToServer, this));
            mENetClient->setDataReceivedCallback(std::bind(&ClientNetworkingSystem::handleServerData, this, std::placeholders::_1));
            mENetClient->setDisconnectedCallback(std::bind(&ClientNetworkingSystem::disconnectedFromServer, this));

            mENetClient->connect(parameters.address, parameters.port, 5000);
        }
        else
        {
            mBus.send(LogMessage{"Could not initialize networking", netName, LogLevel::ERR});
        }
    }
    else if(parameters.mode == NetworkMode::COMBINED)
    {
        mBus.send(LogMessage{"Setting up client networking", netName, LogLevel::INFO});
        mBus.send(LocalConnectionAttemptMessage{&bus});

        mENetClient = std::unique_ptr<ENetClient>(new ENetClient(*mENet, CHANNEL_AMOUNT));
    }
}

void ClientNetworkingSystem::handleMessage(const FrameMessage& received)
{
    if(mENetClient)
        mENetClient->update(0);
}

void ClientNetworkingSystem::handleMessage(const LocalConnectionEstablishedMessage& received)
{
    mServerBus = received.serverBus;
    mBus.send(LogMessage{"Connected locally to server", netName, LogLevel::INFO});

    mBus.send(LogMessage{"Requesting to join game as '" + std::string("Tobbe") + "'", netName, LogLevel::INFO});
    ClientJoinRequestedMessage message{"Tobbe"};
    send(message, true, CHANNEL_DEFAULT);
}

void ClientNetworkingSystem::handleMessage(const ClientJoinDeniedMessage& received)
{
    if(received.reason == JoinDenyReason::FULL)
        mBus.send(LogMessage{"Could not join game at server. Server is full with " + std::to_string(received.playerAmount) + "/" + std::to_string(received.maximumAllowed) + " players", netName, LogLevel::INFO});

    if(mENetClient)
        mENetClient->disconnect(400);
}

void ClientNetworkingSystem::handleMessage(const ClientJoinAcceptedMessage& received)
{
    mBus.send(LogMessage{"Successfully joined the game on server " + received.settings.serverName + "! \nMOTD: " + received.settings.motd, netName, LogLevel::INFO});
}

void ClientNetworkingSystem::connectedToServer()
{
    mBus.send(LogMessage{"Successfully connected to server", netName, LogLevel::INFO});

    mBus.send(LogMessage{"Requesting to join game as '" + std::string("Tobbe") + "'", netName, LogLevel::INFO});
    ClientJoinRequestedMessage message{"Tobbe"};
    send(message, true, CHANNEL_DEFAULT);
}

void ClientNetworkingSystem::handleServerData(const std::vector<uint8_t>& data)
{
    int32_t type = decodeType(data);

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
    else if(type == TEST_1)
        mBus.send(LogMessage{"Received meaningless test message", serverName, LogLevel::WARN});
    else if(type == TEST_2)
        mBus.send(LogMessage{"Received meaningless test message", serverName, LogLevel::WARN});
    else if(type == INVALID)
        mBus.send(LogMessage{"Received invalid message", serverName, LogLevel::WARN});
    else
        mBus.send(LogMessage{"Received message of unknown type", serverName, LogLevel::WARN});
}

void ClientNetworkingSystem::disconnectedFromServer()
{
    mBus.send(LogMessage{"Disconnected from server", netName, LogLevel::INFO});
}
