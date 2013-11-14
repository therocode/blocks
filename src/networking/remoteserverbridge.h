#pragma once
#include "../blockstd.h"
#include "serverclientbridge.h"
#include "enet/enet.h"
#include <thread>
#include <atomic>

class RemoteServerBridge : public ServerClientBridge
{
    public:
		RemoteServerBridge();
        virtual void flush() override;
        //void connect(RemoteServerBridge* other); 
        void receivePackage(std::weak_ptr<BasePackage> incoming);

		void connectToAddress(std::string address, int port = -1);

		void startListening();
		void stopListening();
    private:
		bool mIsHost;
		bool mConnected;
		void createHost();
		void createClient();
		ENetAddress mAddress;
		ENetHost*	mHost;
		int			mPort;
		//At the moment the host is putinto here for clients.
		ENetPeer*	mHostPeer;
		std::thread mThread;
	protected:
		void mListenerFunction();
		std::atomic<bool> mStop;
};