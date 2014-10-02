#include "enetserver.hpp"
#include "../lognames.hpp"
#include "../application/applicationmessages.hpp"
#include "networkingmessages.hpp"
#include "packages.hpp"

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

    //if(mHost != nullptr)
        //mBus.send(LogMessage{"Could not initialize server", netName, LogLevel::ERR});
    //else
        //mBus.send(LogMessage{"Now listening on port " + std::to_string(mAddress.port), netName, LogLevel::INFO});
    update(0);
}

ENetServer::~ENetServer()
{
    if(mHost)
        enet_host_destroy(mHost);
}

void ENetServer::update(uint32_t wait)
{
    if(mHost)
    {
        ENetEvent event;
        while(enet_host_service(mHost, &event, wait) > 0) //0 as third parameter means that we'll not wait for activity. non-blocking
        {
            std::cout << "got event!\n";
            switch(event.type)
            {   
                case ENET_EVENT_TYPE_CONNECT:
                    {
                        std::cout << "incoming connection\n";
                        uint32_t newId = mNextId++;

                        event.peer->data = new uint32_t(newId);
                        mConnectedPeers.emplace(newId, event.peer);

                        mClientCount++;

                        if(mConnectionCallback)
                            mConnectionCallback(newId);

                        break;
                    }
                case ENET_EVENT_TYPE_RECEIVE:
                    {
                        std::cout << "imcoming packeg\n";
                        auto package = deserialize(event.packet);

                        mPackageReceivedCallback(*((uint32_t*) event.peer->data), package);

                        enet_packet_destroy(event.packet);
                        break;
                    }
                case ENET_EVENT_TYPE_DISCONNECT:
                    {
                        std::cout << "incoming disconnection\n";
                        uint32_t* idPtr = (uint32_t*)event.peer->data;

                        if(mDisconnectionCallback)
                            mDisconnectionCallback(*idPtr);

                        mConnectedPeers.erase(*idPtr);
                        delete idPtr;
                        event.peer->data = nullptr;

                        mClientCount--;

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
        
void ENetServer::sendToAll(std::unique_ptr<BasePackage> package)
{
    std::vector<uint8_t> data = package->getAsBytes();

    ENetPacket* packet = enet_packet_create(data.data(), data.size(), package->mUnreliable ? ENET_PACKET_FLAG_UNSEQUENCED : ENET_PACKET_FLAG_RELIABLE);

    for(auto& peer : mConnectedPeers)
        enet_peer_send(peer.second, package->mChannel, packet);
}

void ENetServer::sendToOne(uint32_t id, std::unique_ptr<BasePackage> package)
{
    std::vector<uint8_t> data = package->getAsBytes();

    ENetPacket* packet = enet_packet_create(data.data(), data.size(), package->mUnreliable ? ENET_PACKET_FLAG_UNSEQUENCED : ENET_PACKET_FLAG_RELIABLE);

    FEA_ASSERT(mConnectedPeers.count(id) != 0, "Trying to send a package to enet peer ID " + std::to_string(id) + " but that peer doesn't exist!\n");
    enet_peer_send(mConnectedPeers.at(id), package->mChannel, packet);
}

bool ENetServer::isListening() const
{
    return mHost != nullptr;
}

std::unique_ptr<BasePackage> ENetServer::deserialize(ENetPacket* packet)
{
    PackageType type;

    uint8_t* typePointer = (uint8_t*)&type;
    for(uint32_t i = 0; i < sizeof(PackageType); i++)
    {   
        *typePointer = packet->data[i];
        typePointer++;
    }   

    std::cout << std::to_string((uint32_t)type) << "\n";
    std::vector<uint8_t> dataVector;
    dataVector.resize(packet->dataLength);
    std::copy(packet->data, packet->data + packet->dataLength, dataVector.begin());

    switch(type)
    {   
        case PackageType::REBUILD_SCRIPTS_REQUESTED:
            {   
                std::unique_ptr<BasePackage> packagePtr = std::unique_ptr<RebuildScriptsRequestedPackage>();
                packagePtr->setFromBytes(dataVector);
                return packagePtr;
            }   
        case PackageType::PLAYER_ACTION:
            {
                std::unique_ptr<BasePackage> packagePtr = std::unique_ptr<PlayerActionPackage>();
                packagePtr->setFromBytes(dataVector);
                return packagePtr;
            }
        case PackageType::PLAYER_MOVE_DIRECTION:
            {
                std::unique_ptr<BasePackage> packagePtr = std::unique_ptr<PlayerMoveDirectionPackage>();
                packagePtr->setFromBytes(dataVector);
                return packagePtr;
            }
        case PackageType::PLAYER_MOVE_ACTION:
            {
                std::unique_ptr<BasePackage> packagePtr = std::unique_ptr<PlayerMoveActionPackage>();
                packagePtr->setFromBytes(dataVector);
                return packagePtr;
            }
        case PackageType::PLAYER_PITCH_YAW:
            {
                std::unique_ptr<BasePackage> packagePtr = std::unique_ptr<PlayerPitchYawPackage>();
                packagePtr->setFromBytes(dataVector);
                return packagePtr;
            }
        default:
            if(mUnknownPackageCallback)
                mUnknownPackageCallback(type);
            return std::unique_ptr<BasePackage>();
    }

}

uint32_t ENetServer::getClientCount() const
{
    return mClientCount;
}

void ENetServer::setConnectionCallback(std::function<void(uint32_t clientId)> callback)
{
    mConnectionCallback = callback;
}

void ENetServer::setPackageReceivedCallback(std::function<void(uint32_t, const std::unique_ptr<BasePackage>&)> callback)
{
    mPackageReceivedCallback = callback;
}

void ENetServer::setDisconnectionCallback(std::function<void(uint32_t clientId)> callback)
{
    mDisconnectionCallback = callback;
}

void ENetServer::setUnknownPackageCallback(std::function<void(PackageType)> callback)
{
    mUnknownPackageCallback = callback;
}
