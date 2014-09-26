#include "remoteclientconnectionlistener.hpp"
#include "application/applicationmessages.hpp"

RemoteClientConnectionListener::RemoteClientConnectionListener(fea::MessageBus& bus) : mBus(bus), mLogName("network"), mNextClientId(0)
{
}

void RemoteClientConnectionListener::startListening(int32_t port)
{
    mPort = port;
    mStop = false;
    createHost();
	mListenerThread = std::thread(&RemoteClientConnectionListener::listenerFunction, this);
}

void RemoteClientConnectionListener::listenerFunction()
{
	ENetEvent event;
    while(!mStop)
    {
		while(enet_host_service(mHost, &event, 10) > 0)
		{
			switch(event.type)
            {
                case ENET_EVENT_TYPE_CONNECT:
                    {
                        char ipName[16];
                        enet_address_get_host_ip(&event.peer->address, ipName, 16);
                        mBus.send(LogMessage{"A new client connected from " + std::string(ipName), mLogName, LogLevel::INFO});
                        /* Store any relevant client information here. */
                        RemoteClientBridge* newClientBridge = new RemoteClientBridge(event.peer);
                        size_t newClientId = mNextClientId;
                        mNextClientId++;

                        event.peer->data = new Peer({newClientId, newClientBridge});

                        mPeers.emplace(newClientId, (Peer*)event.peer->data);

                        std::shared_ptr<ClientConnection> clientConnection = std::make_shared<ClientConnection>(newClientId);
                        clientConnection->setBridge(std::unique_ptr<RemoteClientBridge>(newClientBridge));
                        mIncomingConnectionsMutex.lock();
                        mIncomingConnections.push(clientConnection);
                        mIncomingConnectionsMutex.unlock();
                        break;
                    }
                case ENET_EVENT_TYPE_RECEIVE:
                    {
                        //size_t pp = ((int*)event.packet->data)[0];
                        //printf ("A packet of length %u containing %i was received from %s on channel %u.\n",
                        //        (uint32_t)event.packet->dataLength,
                        //        (uint32_t)pp,
                        //        event.peer->data,
                        //        event.channelID);
                        /* Clean up the packet now that we're done using it. */
                        ((Peer*)event.peer->data)->mBridge->acceptEnetPacket(event.packet);
                        enet_packet_destroy (event.packet);
                        break;
                    }
                case ENET_EVENT_TYPE_DISCONNECT:
                    {
                        printf("%d disconnected.\n", (int)((Peer*)event.peer->data)->mId);
                        /* Reset the peer's client information. */
                        mPeers.erase(((Peer*)event.peer->data)->mId);
                        ((Peer*)event.peer->data)->mBridge->disconnect();
                        //mDisconnectingmutex
                        //mDisconnecting.push()
                        delete (Peer*)event.peer->data;
                        event.peer->data = nullptr;
                        break;
                    }
                case ENET_EVENT_TYPE_NONE:
                {
                    break;
                }
            }
		}
		enet_host_flush(mHost);
        for(auto peers : mPeers)
        {
            peers.second->mBridge->sendAwaiting();
        }
    }
}

void RemoteClientConnectionListener::stopListening()
{
    mStop = true;
    mListenerThread.join();
}

void RemoteClientConnectionListener::createHost()
{
	//	enet_address_set_host(&mAddress, "localhost");
	mAddress.host = ENET_HOST_ANY;
	mAddress.port = mPort;
	mHost = enet_host_create(&mAddress, //What address to host on.
			32,		//Maximum connections.
			CHANNEL_COUNT,		//Maximum channels.
			0,		//Estimated incoming traffic.
			0);		//Estimated outcoming traffic.
	if(mHost == NULL)
    {
		printf("Server couldn't create, port already in use.\n");
        mBus.send(LogMessage{"Cannot listen on port " + std::to_string(mPort) + ", port already in use!", mLogName, LogLevel::ERR});
		exit(1);
	}
    
    else
	{
        mBus.send(LogMessage{"Now listening on port " + std::to_string(mPort) + " for incoming connections.", mLogName, LogLevel::INFO});
	}
}
