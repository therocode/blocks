#include "localclientconnectionlistener.h"

void LocalClientConnectionListener::createClientConnection(LocalServerClientBridge* clientBridge)
{
    std::shared_ptr<ClientConnection> clientConnection = std::make_shared<ClientConnection>();

    LocalServerClientBridge* serverBridge = new LocalServerClientBridge();

    serverBridge->connect(clientBridge);
    clientBridge->connect(serverBridge);

    clientConnection->setBridge(std::unique_ptr<LocalServerClientBridge>(serverBridge));

    mIncomingConnections.push(clientConnection);
}
