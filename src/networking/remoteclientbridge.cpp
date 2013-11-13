#include "remoteclientbridge.h"
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
    //parse ENet packet and deserialise to put into receivePackage();
}

void RemoteClientBridge::receivePackage(std::weak_ptr<BasePackage> incoming)
{
	mIncoming.push_back(incoming.lock());
}
