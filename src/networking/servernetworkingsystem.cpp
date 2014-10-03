#include "servernetworkingsystem.hpp"
#include "../world/chunk.hpp"
#include "../lognames.hpp"
#include "../script/scriptmessages.hpp"
#include "../input/inputmessages.hpp"

ServerNetworkingSystem::ServerNetworkingSystem(fea::MessageBus& bus, const NetworkParameters& parameters) :
    mBus(bus),
    mParameters(parameters),
    mNextClientId(0),
    mLocalPlayerId(0),
    mLocalClientBus(nullptr)
{

    subscribe(mBus, *this);

    if(parameters.mode == NetworkMode::SINGLE_PLAYER)
    {
        mBus.send(LogMessage{"Setting up single player networking", netName, LogLevel::INFO});
    }
    else if(parameters.mode == NetworkMode::DEDICATED)
    {
        mENet = std::unique_ptr<ENet>(new ENet());

        if(mENet->isInitialized())
        {
            mBus.send(LogMessage{"Setting up dedicated server networking", netName, LogLevel::INFO});

            mENetServer = std::unique_ptr<ENetServer>(new ENetServer(*mENet, parameters.port));
            mENetServer->setConnectedCallback(std::bind(&ServerNetworkingSystem::acceptRemoteClient, this, std::placeholders::_1));
            mENetServer->setDataReceivedCallback(std::bind(&ServerNetworkingSystem::handleClientData, this, std::placeholders::_1, std::placeholders::_2));
            mENetServer->setDisconnectedCallback(std::bind(&ServerNetworkingSystem::disconnectRemoteClient, this, std::placeholders::_1));

            if(mENetServer->isListening())
                mBus.send(LogMessage{"Now listening on port " + std::to_string(parameters.port), netName, LogLevel::INFO});
            else
                mBus.send(LogMessage{"Could not bind to port " + std::to_string(parameters.port), netName, LogLevel::ERR});
        }
        else
        {
            mBus.send(LogMessage{"Could not initialize networking", netName, LogLevel::ERR});
        }
    }
    else if(parameters.mode == NetworkMode::COMBINED)
    {
        mENet = std::unique_ptr<ENet>(new ENet());

        if(mENet->isInitialized())
        {
            mBus.send(LogMessage{"Setting up networking", netName, LogLevel::INFO});

            mENetServer = std::unique_ptr<ENetServer>(new ENetServer(*mENet, parameters.port));
            mENetServer = std::unique_ptr<ENetServer>(new ENetServer(*mENet, parameters.port));
            mENetServer->setConnectedCallback(std::bind(&ServerNetworkingSystem::acceptRemoteClient, this, std::placeholders::_1));
            mENetServer->setDataReceivedCallback(std::bind(&ServerNetworkingSystem::handleClientData, this, std::placeholders::_1, std::placeholders::_2));
            mENetServer->setDisconnectedCallback(std::bind(&ServerNetworkingSystem::disconnectRemoteClient, this, std::placeholders::_1));

            if(mENetServer->isListening())
                mBus.send(LogMessage{"Now listening on port " + std::to_string(parameters.port), netName, LogLevel::INFO});
            else
                mBus.send(LogMessage{"Could not bind to port " + std::to_string(parameters.port), netName, LogLevel::ERR});
        }
        else
        {
            mBus.send(LogMessage{"Could not initialize networking", netName, LogLevel::ERR});
        }
    }
}

void ServerNetworkingSystem::handleMessage(const LocalConnectionAttemptMessage& received)
{
    uint32_t newId = mNextClientId++;

    mLocalPlayerId = newId;
    mLocalClientBus = received.clientBus;

    mBus.send(LogMessage{"Client connected locally and given player Id " + std::to_string(newId), netName, LogLevel::INFO});
    mBus.send(PlayerJoinedMessage{newId, 0, {0.0f, 0.0f, 0.0f}});

    mBus.send(LocalConnectionEstablishedMessage{&mBus});
}

void ServerNetworkingSystem::handleMessage(const FrameMessage& received)
{
    if(mENetServer)
        mENetServer->update(0);
}

void ServerNetworkingSystem::acceptRemoteClient(uint32_t id)
{
    uint32_t newId = mNextClientId++;

    mClientToPlayerIds.emplace(id, newId);
    mPlayerToClientIds.emplace(newId, id);

    mBus.send(LogMessage{"Client Id " + std::to_string(id) + " connected and given player Id " + std::to_string(newId), netName, LogLevel::INFO});
    mBus.send(PlayerJoinedMessage{newId, 0, {0.0f, 0.0f, 0.0f}});
}

void ServerNetworkingSystem::handleClientData(uint32_t clientId, const std::vector<uint8_t>& data)
{
    //PackageType type;

    //uint8_t* typePointer = (uint8_t*)&type;
    //for(uint32_t i = 0; i < sizeof(PackageType); i++)
    //{   
    //    *typePointer = data[i];
    //    typePointer++;
    //}   
}

void ServerNetworkingSystem::disconnectRemoteClient(uint32_t id)
{
    FEA_ASSERT(mClientToPlayerIds.count(id) != 0, "Client " << id << " disconnecting, but that client is not marked as connected!");

    uint32_t playerId = mClientToPlayerIds.at(id);

    mBus.send(LogMessage{"Client Id " + std::to_string(id) + ", player Id " + std::to_string(playerId) + " disconnected", netName, LogLevel::INFO});
    mBus.send(PlayerDisconnectedMessage{playerId});

    mClientToPlayerIds.erase(id);
    mPlayerToClientIds.erase(playerId);
}
