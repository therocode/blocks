#pragma once
#include "../blockstd.h"
#include "serverclientbridge.h"
#include "enet/enet.h"
#include <thread>
#include <atomic>

class RemoteServerClientBridge : public ServerClientBridge
{
    public:
		RemoteServerClientBridge(bool isServer);
        virtual void flush() override;
        void connect(RemoteServerClientBridge* other); 
        void receivePackage(std::unique_ptr<BasePackage>&& incoming);

		void connectToAddress(std::string address);

		void startListening();
		void stopListening();
    private:
		bool mIsHost;
		bool mConnected;
		static bool sEnetInitialized;
		void createHost();
		void createClient();
        RemoteServerClientBridge* mOther;
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
