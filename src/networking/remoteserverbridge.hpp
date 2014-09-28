#pragma once
#include "../blockstd.hpp"
#include "serverclientbridge.hpp"
#include <enet/enet.h>
#include <thread>
#include <atomic>
#include <fea/util.hpp>

class RemoteServerBridge : public ServerClientBridge
{
    public:
		RemoteServerBridge(fea::MessageBus& bus);
        ~RemoteServerBridge();
        virtual void flush() override;
        void connect(RemoteServerBridge* other);
        void receivePackage(std::weak_ptr<BasePackage> incoming);

		void connectToAddress(std::string address, int port = -1);

		void startListening();
		void stopListening();
    private:
        fea::MessageBus& mBus;
		bool mConnected;
		void createClient();
		ENetAddress mAddress;
		ENetHost*	mHost;
		int			mPort;
		//At the moment the host is putinto here for clients.
		ENetPeer*	mHostPeer;
		std::thread mThread;
        void acceptEnetPacket(ENetPacket* packet);
        void deserialiseAndReceive(const std::vector<uint8_t>& data, BasePackage* package);
	protected:
        int  mChannelCount;
		void mListenerFunction();
		std::atomic<bool> mStop;

        std::atomic<bool> mGotPackagesToSend;
};
