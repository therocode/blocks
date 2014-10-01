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

    //if(mHost == nullptr)
    //    std::cout << "failed to create client\n";//mBus.send(LogMessage{"Could not initialize client", netName, LogLevel::ERR});
    //else
    //{
    //    std::cout << "success to create client\n";
    //}
}

ENetClient::~ENetClient()
{
    //mBus.send(LogMessage{"Shutting down ENet", netName, LogLevel::ERR});

    if(mHost)
        enet_host_destroy(mHost);
}

bool ENetClient::isConnected() const
{
    return mConnected;
}

void ENetClient::connect(const std::string& address, uint32_t port, uint32_t timeout)
{
    int result = enet_address_set_host(&mAddress, address.c_str());
    mAddress.port = port;

    mServer = enet_host_connect(mHost, &mAddress, 2, 0);

    ENetEvent event;

    if(enet_host_service(mHost, &event, timeout) > 0 &&
            event.type == ENET_EVENT_TYPE_CONNECT)
    {
        mConnected = true;
    }
}

void ENetClient::disconnect()
{
    enet_peer_disconnect(mServer, 0);
    mConnected = false;
}
