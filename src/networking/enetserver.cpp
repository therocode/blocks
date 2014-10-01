#include "enetserver.hpp"
#include "../lognames.hpp"
#include "../application/applicationmessages.hpp"
#include "networkingmessages.hpp"
#include "packages.hpp"

ENetServer::ENetServer(const ENet& enet, fea::MessageBus& bus) :
    mBus(bus),
    mHost(nullptr),
    mInitialized(false),
    mNextId(0)
{
    (void)enet;

    mBus.send(LogMessage{"ENet initialized", netName, LogLevel::INFO});
    mInitialized = true;

    mAddress.host = ENET_HOST_ANY;
    mAddress.port = 56556;

    mHost = enet_host_create(&mAddress,  //address to bind this server to
            32,                          //allow 32 incoming connections
            2,                           //allow 2 channels
            0,                           //no downstream limit
            0);                          //no upstream limit

    if(mHost == nullptr)
        mBus.send(LogMessage{"Could not initialize server", netName, LogLevel::ERR});
    else
        mBus.send(LogMessage{"Now listening on port " + std::to_string(mAddress.port), netName, LogLevel::INFO});
}

ENetServer::~ENetServer()
{
    mBus.send(LogMessage{"Shutting down ENet", netName, LogLevel::ERR});

    if(mHost)
        enet_host_destroy(mHost);

    if(mInitialized)
        enet_deinitialize();
}

void ENetServer::update()
{
    if(mHost)
    {
        ENetEvent event;

        while(enet_host_service(mHost, &event, 0) > 0) //0 as third parameter means that we'll not wait for activity. non-blocking
        {
            switch(event.type)
            {   
                case ENET_EVENT_TYPE_CONNECT:
                    {
                        mBus.send(LogMessage{"new client connected from " + std::to_string(event.peer->address.host) + ":" + std::to_string(event.peer->address.port), netName, LogLevel::INFO});

                        uint32_t newId = mNextId++;

                        event.peer->data = new uint32_t(newId);
                        mConnectedPeers.emplace(newId, event.peer);

                        mBus.send(IncomingConnectionMessage{newId});

                        break;
                    }
                case ENET_EVENT_TYPE_RECEIVE:
                    {
                        mBus.send(LogMessage{"a packet of length " + std::to_string(event.packet->dataLength) + " was received from " + std::to_string(event.peer->address.host) + " on channel " + std::to_string(event.channelID), netName, LogLevel::VERB});

                        auto package = deserialize(event.packet);

                        mBus.send(ClientPackageReceived{*((uint32_t*) event.peer->data), package});

                        enet_packet_destroy(event.packet);
                        break;
                    }
                case ENET_EVENT_TYPE_DISCONNECT:
                    {
                        mBus.send(LogMessage{"client from " + std::to_string(event.peer->address.host) + ":" + std::to_string(event.peer->address.port) + " disconnected", netName, LogLevel::INFO});

                        uint32_t* idPtr = (uint32_t*)event.peer->data;

                        mBus.send(ClientDisconnectedMessage{*idPtr});

                        mConnectedPeers.erase(*idPtr);
                        delete idPtr;
                        event.peer->data = nullptr;
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
            mBus.send(LogMessage{"Unknown package received", netName, LogLevel::ERR});
            return std::unique_ptr<BasePackage>();
            break;
    }

}
