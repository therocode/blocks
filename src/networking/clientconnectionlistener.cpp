#include "clientconnectionlistener.h"

std::shared_ptr<ClientConnection> ClientConnectionListener::fetchIncomingConnection()
{
   if(mIncomingConnections.size() > 0)
   {
       std::shared_ptr<ClientConnection> returnVal = mIncomingConnections.front();
       mIncomingConnections.pop();
       return returnVal;
   }
   else
   {
       return nullptr;
   }
}
