#include "enetclient.hpp"
#include "networkingmessages.hpp"
#include "../application/applicationmessages.hpp"
#include "../lognames.hpp"
        
ENetClient::ENetClient(const ENet& enet, fea::MessageBus& bus) :
    mBus(bus),
    mHost(nullptr)
{
    (void)enet;

    mHost = enet_host_create(nullptr,    //nullptr since this is a client
            1,                           //allow 1 outgoing connection
            2,                           //allow 2 channels
            0,                           //no downstream limit
            0);                          //no upstream limit

    if(mHost == nullptr)
        mBus.send(LogMessage{"Could not initialize client", netName, LogLevel::ERR});
    else
    {
        ENetAddress address;
        enet_address_set_host(&address, "localhost");
        address.port = 56556;

        mServer = enet_host_connect(mHost, &address, 2, 0);

        mBus.send(LogMessage{"Now listening on port " + std::to_string(address.port), netName, LogLevel::INFO});
    }
}

ENetClient::~ENetClient()
{
    mBus.send(LogMessage{"Shutting down ENet", netName, LogLevel::ERR});

    if(mHost)
        enet_host_destroy(mHost);
}
