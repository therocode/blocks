#include "clientconnectionlistener.hpp"

std::shared_ptr<ClientConnection> ClientConnectionListener::fetchIncomingConnection()
{
   std::lock_guard<std::mutex> lock(mIncomingConnectionsMutex);

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
