#include "listener.hpp"

std::unique_ptr<ClientConnection> Listener::fetchIncomingConnection()
{
   std::lock_guard<std::mutex> lock(mIncomingConnectionsMutex);

   if(mIncomingConnections.size() > 0)
   {
       auto returnVal = std::move(mIncomingConnections.front());
       mIncomingConnections.pop();
       return returnVal;
   }
   else
   {
       return nullptr;
   }
}
