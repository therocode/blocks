#include "localclientconnectionlistener.h"

void LocalClientConnectionListener::createClientConnection(LocalServerClientBridge* clientBridge)
{
    std::shared_ptr<ClientConnection> clientConnection = std::make_shared<ClientConnection>(0); //local client will always have id 0

    LocalServerClientBridge* serverBridge = new LocalServerClientBridge();

    serverBridge->connect(clientBridge);
    clientBridge->connect(serverBridge);

    clientConnection->setBridge(std::unique_ptr<LocalServerClientBridge>(serverBridge));

    mIncomingConnectionsMutex.lock();
    mIncomingConnections.push(clientConnection);
    mIncomingConnectionsMutex.unlock();
}
