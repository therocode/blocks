#include "enetclient.hpp"
#include "networkingmessages.hpp"
#include "../application/applicationmessages.hpp"
#include "../lognames.hpp"
        
ENetClient::ENetClient(const ENet& enet) :
    mHost(nullptr),
    mServer(nullptr),
    mConnected(false)
{
    (void)enet;

    mHost = enet_host_create(nullptr,    //nullptr since this is a client
            1,                           //allow 1 outgoing connection
            2,                           //allow 2 channels
            0,                           //no downstream limit
            0);                          //no upstream limit
}

ENetClient::~ENetClient()
{
    if(mHost)
    {
        if(isConnected())
            disconnect(1000);
        enet_host_destroy(mHost);
    }
}

void ENetClient::update(uint32_t wait)
{
    if(mHost)
    {
        ENetEvent event;
        while(enet_host_service(mHost, &event, wait) > 0) //0 as third parameter means that we'll not wait for activity. non-blocking
        {
            switch(event.type)
            {   
                case ENET_EVENT_TYPE_CONNECT:
                    {
                        //shouldn't be handled since this is a client
                        break;
                    }
                case ENET_EVENT_TYPE_RECEIVE:
                    {
                        auto package = unpack(event.packet);

                        if(mDataReceivedCallback)
                            mDataReceivedCallback(package);

                        enet_packet_destroy(event.packet);
                        break;
                    }
                case ENET_EVENT_TYPE_DISCONNECT:
                    {
                        if(mDisconnectedCallback)
                            mDisconnectedCallback();

                        mConnected = false;
                        mServer = nullptr;
                            
                        break;
                    }
                case ENET_EVENT_TYPE_NONE:
                    {
                        break;
                    }
            }   
        }
    }
}

bool ENetClient::isConnected() const
{
    return mConnected;
}

void ENetClient::connect(const std::string& address, uint32_t port, uint32_t timeout)
{
    FEA_ASSERT(!mConnected, "Trying to connect to a server whilst already connected!");

    int result = enet_address_set_host(&mAddress, address.c_str());
    mAddress.port = port;

    mServer = enet_host_connect(mHost, &mAddress, 2, 0);

    ENetEvent event;

        std::cout << "connecting to " << address << ":" << port << "\n";
        
    if(enet_host_service(mHost, &event, timeout) > 0 &&
            event.type == ENET_EVENT_TYPE_CONNECT)
    {
        if(mConnectedCallback)
            mConnectedCallback();

        std::cout << "connected!\n";
        mConnected = true;
    }

    std::cout << "step over\n";
}

void ENetClient::disconnect(uint32_t timeout)
{
    if(mServer)
    {
        enet_peer_disconnect(mServer, 0);

        ENetEvent event;

        while(enet_host_service(mHost, &event, timeout) > 0)
        {
            switch(event.type)
            {
                case ENET_EVENT_TYPE_RECEIVE:
                    enet_packet_destroy(event.packet);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:

                    if(mDisconnectedCallback)
                        mDisconnectedCallback();

                    mConnected = false;
                    mServer = nullptr;
                    return;
                default:
                    break;
            }
        }

        enet_peer_reset(mServer);
        mConnected = false;
        mServer = nullptr;
    }
}

void ENetClient::send(const std::vector<uint8_t>& data, bool reliable, uint8_t channel)
{
    FEA_ASSERT(mServer != nullptr, "Trying to send to a server but not connected to any server!");
    
    ENetPacket* packet = enet_packet_create(data.data(), data.size(), reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNSEQUENCED);

    enet_peer_send(mServer, channel, packet);
}

void ENetClient::setConnectedCallback(std::function<void()> callback)
{
    mConnectedCallback = callback;
}

void ENetClient::setDataReceivedCallback(std::function<void(const std::vector<uint8_t>&)> callback)
{
    mDataReceivedCallback = callback;
}

void ENetClient::setDisconnectedCallback(std::function<void()> callback)
{
    mDisconnectedCallback = callback;
}

std::vector<uint8_t> ENetClient::unpack(ENetPacket* packet)
{
    std::vector<uint8_t> dataVector;
    dataVector.resize(packet->dataLength);
    std::copy(packet->data, packet->data + packet->dataLength, dataVector.begin());

    return dataVector;
}
