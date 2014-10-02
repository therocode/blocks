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
    FEA_ASSERT(!mConnected, "Trying to connect to a server whilst already connected!");
    int result = enet_address_set_host(&mAddress, address.c_str());
    mAddress.port = port;

    mServer = enet_host_connect(mHost, &mAddress, 2, 0);

    ENetEvent event;

    if(enet_host_service(mHost, &event, timeout) > 0 &&
            event.type == ENET_EVENT_TYPE_CONNECT)
    {
        std::cout << "client coonnected to " << mServer << "\n";
        mConnected = true;
    }
}

void ENetClient::disconnect(uint32_t timeout)
{
    std::cout << "will maybe disconnect from server " << mServer << "\n";
    if(mServer)
    {
        std::cout << "attempting disconnection\n";
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
                    mConnected = false;
                    mServer = nullptr;
                    std::cout << "disconnected gracefully\n";
                    return;
                default:
                    break;
            }
        }

        std::cout << "oh no, will have to force it\n";
        enet_peer_reset(mServer);
        mConnected = false;
        mServer = nullptr;
    }
}
