#include "localclientlistener.hpp"
#include "localserverclientbridge.hpp"

void LocalClientListener::createClientConnection(LocalServerClientBridge* clientBridge)
{
    LocalServerClientBridge* rawServerBridge = new LocalServerClientBridge();
    std::unique_ptr<ServerClientBridge> serverBridge = std::unique_ptr<LocalServerClientBridge>(rawServerBridge);

    rawServerBridge->connect(clientBridge);
    clientBridge->connect(rawServerBridge);

    std::lock_guard<std::mutex> lock(mIncomingConnectionsMutex);
    mIncomingConnections.push(std::move(serverBridge));
}
