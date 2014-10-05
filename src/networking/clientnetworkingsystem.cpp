#include "clientnetworkingsystem.hpp"
#include "networkingmessages.hpp"
#include "../world/worldmessages.hpp"
#include "../lognames.hpp"
#include "enetclient.hpp"
#include "networkingprotocol.hpp"

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

            mENetClient = std::unique_ptr<ENetClient>(new ENetClient(*mENet));
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

        mENetClient = std::unique_ptr<ENetClient>(new ENetClient(*mENet));
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
}

void ClientNetworkingSystem::connectedToServer()
{
    mBus.send(LogMessage{"Successfully connected to server", netName, LogLevel::INFO});
}

void ClientNetworkingSystem::handleServerData(const std::vector<uint8_t>& data)
{
    //PackageType type;

    //uint8_t* typePointer = (uint8_t*)&type;
    //for(uint32_t i = 0; i < sizeof(PackageType); i++)
    //{   
    //    *typePointer = data[i];
    //    typePointer++;
    //}   
}

void ClientNetworkingSystem::disconnectedFromServer()
{
    mBus.send(LogMessage{"Disconnected from server", netName, LogLevel::INFO});
}
