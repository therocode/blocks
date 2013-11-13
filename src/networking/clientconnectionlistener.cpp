#include "clientconnectionlistener.h"

std::shared_ptr<ClientConnection> ClientConnectionListener::fetchIncomingConnection()
{
    mIncomingConnectionsMutex.lock();
   if(mIncomingConnections.size() > 0)
   {
       std::shared_ptr<ClientConnection> returnVal = mIncomingConnections.front();
       mIncomingConnections.pop();
       mIncomingConnectionsMutex.unlock();
       return returnVal;
   }
   else
   {
       mIncomingConnectionsMutex.unlock();
       return nullptr;
   }
}
