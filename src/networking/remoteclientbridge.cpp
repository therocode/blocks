#include "remoteclientbridge.h"
#include "packages.h"
#include <iostream>

RemoteClientBridge::RemoteClientBridge(ENetPeer* peer) : mPeer(peer)
{
}

void RemoteClientBridge::flush()
{
    //grab packages from the outgoing queue, serialise and pack them up and send them to the enet peer
}

void RemoteClientBridge::acceptEnetPacket(ENetPacket* packet)
{
    std::cout << "will do stuff\n";
    //parse ENet packet and deserialise to put into receivePackage();
            //(uint32_t)packet->dataLength,

    PackageType type;

    uint8_t* typePointer = (uint8_t*)&type;
    for(uint32_t i = 0; i < sizeof(PackageType); i++)
    {
        *typePointer = packet->data[i];
        typePointer++;
    }
    
    std::cout << "type is " << (int32_t)type << "\n";
    std::vector<uint8_t> dataVector;
    dataVector.resize(packet->dataLength);
    std::copy(packet->data, packet->data + packet->dataLength, dataVector.begin());

    std::shared_ptr<BasePackage> packagePtr;

    switch(type)
    {
        case PackageType::PLAYER_MOVE_ACTION:
        {
            PlayerMoveActionPackage* package = new PlayerMoveActionPackage();
            package->deserialise(dataVector);
            packagePtr = std::shared_ptr<BasePackage>(package);
            receivePackage(packagePtr);
            break;
        }
        case PackageType::PLAYER_MOVE_DIRECTION:
        {
            PlayerMoveDirectionPackage* package = new PlayerMoveDirectionPackage();
            package->deserialise(dataVector);
            packagePtr = std::shared_ptr<BasePackage>(package);
            receivePackage(packagePtr);
            break;
        }
    }
}

void RemoteClientBridge::receivePackage(std::weak_ptr<BasePackage> incoming)
{
	mIncoming.push_back(incoming.lock());
}
