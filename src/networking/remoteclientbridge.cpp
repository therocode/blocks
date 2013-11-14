#include "remoteclientbridge.h"
#include "packages.h"
#include <iostream>

RemoteClientBridge::RemoteClientBridge(ENetPeer* peer) : mPeer(peer)
{
}

void RemoteClientBridge::flush()
{
    //grab packages from the outgoing queue, serialise and pack them up and send them to the enet peer
    for(auto package : mOutgoing)
    {
        std::vector<uint8_t> data = package->serialise();

        ENetPacket* packet = enet_packet_create(&data[0], data.size(), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(mPeer, 0, packet);
    }
    //enet_host_flush(mPeer);
    mOutgoing.clear();
}

void RemoteClientBridge::acceptEnetPacket(ENetPacket* packet)
{
    //parse ENet packet and deserialise to put into receivePackage();
            //(uint32_t)packet->dataLength,

    PackageType type;

    uint8_t* typePointer = (uint8_t*)&type;
    for(uint32_t i = 0; i < sizeof(PackageType); i++)
    {
        *typePointer = packet->data[i];
        typePointer++;
    }

    std::vector<uint8_t> dataVector;
    dataVector.resize(packet->dataLength);
    std::copy(packet->data, packet->data + packet->dataLength, dataVector.begin());


    switch(type)
    {
        case PackageType::REBUILD_SCRIPTS_REQUESTED:
        {
            deserialiseAndReceive(dataVector, new RebuildScriptsRequestedPackage());
            break;
        }
        case PackageType::PLAYER_ACTION:
        {
            deserialiseAndReceive(dataVector, new PlayerActionPackage());
            break;
        }
        case PackageType::PLAYER_PITCH_YAW:
        {
            deserialiseAndReceive(dataVector, new PlayerPitchYawPackage());
            break;
        }
        case PackageType::PLAYER_MOVE_ACTION:
        {
            deserialiseAndReceive(dataVector, new PlayerMoveActionPackage());
            break;
        }
        case PackageType::PLAYER_MOVE_DIRECTION:
        {
            deserialiseAndReceive(dataVector, new PlayerMoveDirectionPackage());
            break;
        }
    }
}

void RemoteClientBridge::receivePackage(std::weak_ptr<BasePackage> incoming)
{
	mIncoming.push_back(incoming.lock());
}

void RemoteClientBridge::deserialiseAndReceive(const std::vector<uint8_t>& data, BasePackage* package)
{
    package->deserialise(data);
    std::shared_ptr<BasePackage> packagePtr = std::shared_ptr<BasePackage>(package);
    receivePackage(packagePtr);
}
