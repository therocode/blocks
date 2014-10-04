#include "enetserver.hpp"
#include "../lognames.hpp"
#include "../application/applicationmessages.hpp"
#include "networkingmessages.hpp"

ENetServer::ENetServer(const ENet& enet, uint32_t port) :
    mHost(nullptr),
    mNextId(0),
    mClientCount(0)
{
    (void)enet;

    mAddress.host = ENET_HOST_ANY;
    mAddress.port = port;

    mHost = enet_host_create(&mAddress,  //address to bind this server to
            32,                          //allow 32 incoming connections
            2,                           //allow 2 channels
            0,                           //no downstream limit
            0);                          //no upstream limit
}

ENetServer::~ENetServer()
{
    if(mHost)
    {
        disconnectAll(500);
        enet_host_destroy(mHost);
    }
}

void ENetServer::update(uint32_t wait)
{
    if(mHost)
    {
        ENetEvent event;
        while(enet_host_service(mHost, &event, wait) > 0) //0 as third parameter means that we'll not wait for activity. non-blocking
        {
            switch(event.type)
            {   
                case ENET_EVENT_TYPE_CONNECT:
                        handleConnectionEvent(event);
                        break;
                    
                case ENET_EVENT_TYPE_RECEIVE:
                        handleReceiveEvent(event);
                        break;
                    
                case ENET_EVENT_TYPE_DISCONNECT:
                        handleDisconnectionEvent(event);
                        break;
                    
                case ENET_EVENT_TYPE_NONE:
                        break;
            }   
        }
    }
}
        
void ENetServer::sendToAll(const std::vector<uint8_t>& data, bool reliable, uint8_t channel)
{
    ENetPacket* packet = enet_packet_create(data.data(), data.size(), reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNSEQUENCED);

    for(auto& peer : mConnectedPeers)
        enet_peer_send(peer.second, channel, packet);
}

void ENetServer::sendToOne(uint32_t id, const std::vector<uint8_t>& data, bool reliable, uint8_t channel)
{
    ENetPacket* packet = enet_packet_create(data.data(), data.size(), reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNSEQUENCED);

    FEA_ASSERT(mConnectedPeers.count(id) != 0, "Trying to send a package to enet peer ID " + std::to_string(id) + " but that peer doesn't exist!\n");

    enet_peer_send(mConnectedPeers.at(id), channel, packet);
}

void ENetServer::disconnectOne(uint32_t id, uint32_t wait)
{
    ENetEvent event;

    FEA_ASSERT(mConnectedPeers.count(id) != 0, "Trying to disconnect enet peer ID " + std::to_string(id) + " but that peer doesn't exist!\n");

    ENetPeer* client = mConnectedPeers.at(id);

    enet_peer_disconnect(client, 0);

    bool disconnected = false;
     
    while(enet_host_service(mHost, & event, wait) > 0)
    {
        switch (event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
                handleConnectionEvent(event);
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                if(event.peer != client)
                    handleReceiveEvent(event); //still handle packets from other clients than the one being disconnected
                else
                    enet_packet_destroy(event.packet);

                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                if(event.peer == client)
                    disconnected = true;

                handleDisconnectionEvent(event);
                break;

            default:
                break;
        }
    }

    /* if disconnection didn't succeed yet, force the connection down.             */
    if(!disconnected)
        enet_peer_reset(client);
}

void ENetServer::disconnectAll(uint32_t wait)
{
    std::vector<uint32_t> idsToDisconnect;

    for(const auto& peer : mConnectedPeers)
        idsToDisconnect.push_back(peer.first);

    for(uint32_t id : idsToDisconnect)
        disconnectOne(id, wait);
}

bool ENetServer::isListening() const
{
    return mHost != nullptr;
}

uint32_t ENetServer::getClientCount() const
{
    return mClientCount;
}

void ENetServer::setConnectedCallback(std::function<void(uint32_t)> callback)
{
    mConnectedCallback = callback;
}

void ENetServer::setDataReceivedCallback(std::function<void(uint32_t, const std::vector<uint8_t>&)> callback)
{
    mDataReceivedCallback = callback;
}

void ENetServer::setDisconnectedCallback(std::function<void(uint32_t)> callback)
{
    mDisconnectedCallback = callback;
}

std::vector<uint8_t> ENetServer::unpack(ENetPacket* packet)
{
    std::vector<uint8_t> dataVector;
    dataVector.resize(packet->dataLength);
    std::copy(packet->data, packet->data + packet->dataLength, dataVector.begin());

    return dataVector;
}

void ENetServer::handleConnectionEvent(const ENetEvent& event)
{
    uint32_t newId = mNextId++;

    event.peer->data = new uint32_t(newId);
    mConnectedPeers.emplace(newId, event.peer);

    mClientCount++;

    if(mConnectedCallback)
        mConnectedCallback(newId);
}

void ENetServer::handleReceiveEvent(const ENetEvent& event)
{
    auto package = unpack(event.packet);

    if(mDataReceivedCallback)
        mDataReceivedCallback(*((uint32_t*) event.peer->data), package);

    enet_packet_destroy(event.packet);
}

void ENetServer::handleDisconnectionEvent(const ENetEvent& event)
{
    uint32_t* idPtr = (uint32_t*)event.peer->data;

    if(mDisconnectedCallback)
        mDisconnectedCallback(*idPtr);

    mConnectedPeers.erase(*idPtr);
    delete idPtr;
    event.peer->data = nullptr;

    mClientCount--;
}
