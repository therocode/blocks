#pragma once
#include "clientconnectionlistener.hpp"
#include "remoteclientbridge.hpp"
#include <thread>
#include <atomic>
#include <fea/util.hpp>
#include "packages.hpp"
#include <enet/enet.h>

class RemoteClientConnectionListener : public ClientConnectionListener
{
    struct Peer
    {
        size_t mId;
        RemoteClientBridge* mBridge;
    };

    public:
        RemoteClientConnectionListener(fea::MessageBus& bus);
        void startListening(int32_t port);
        void listenerFunction();
        void stopListening();
    private:
        void createHost();
        fea::MessageBus& mBus;
		std::thread mListenerThread;
		ENetAddress mAddress;
		ENetHost* mHost;
        int32_t mPort;
		std::atomic<bool> mStop;
        std::atomic<size_t> mNextClientId;
        std::unordered_map<size_t, Peer*> mPeers;
};
