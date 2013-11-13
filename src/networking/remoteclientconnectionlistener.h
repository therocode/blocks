#pragma once
#include "clientconnectionlistener.h"
#include "remoteclientbridge.h"
#include "enet/enet.h"
#include <thread>
#include <atomic>

class RemoteClientConnectionListener : public ClientConnectionListener
{
    struct Peer
    {
        size_t mId;
        RemoteClientBridge* mBridge;
    };

    public:
        RemoteClientConnectionListener();
        void startListening();
        void listenerFunction();
        void stopListening();
    private:
        void createHost();
		std::thread mListenerThread;
		ENetAddress mAddress;
		ENetHost* mHost;
        int32_t mPort;
		std::atomic<bool> mStop;
        std::atomic<size_t> mNextClientId;
};
