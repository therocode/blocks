#include "remoteserverclientbridge.h"
#include <iostream>

bool RemoteServerClientBridge::sEnetInitialized = false;

RemoteServerClientBridge::RemoteServerClientBridge(bool isServer)
{
	mIsHost = isServer;
	mConnected = false;
	if(!RemoteServerClientBridge::sEnetInitialized)
	{
		if(enet_initialize() < 0)
		{
			printf("ENet failed to initialize\n");
		}
		RemoteServerClientBridge::sEnetInitialized = true;
	}
	mPort = 35940;
}
void RemoteServerClientBridge::connectToAddress(std::string address, int port)
{
	printf("Going to try to connect to %s\n", address.c_str());
	if(!mIsHost && !mConnected)
	{
		createClient();
		enet_address_set_host(&mAddress, address.c_str());	
		if(port != -1)
		{
			mPort = port;
		}
		mAddress.port = mPort;

		mHostPeer = enet_host_connect(mHost, &mAddress, 2, 0);

		if(mHostPeer == NULL)
		{
			printf("couldn't create connection peer\n");
		}

		ENetEvent event;
		if(enet_host_service(mHost, &event, 5000) > 0 &&
				event.type == ENET_EVENT_TYPE_CONNECT)
		{
			printf("Wow, we really connected to %s.\n", address.c_str());
			//When connected, greet the server!
			int i[4];
			for(int o = 1; o < 4; o++)i[o] = (int)(64 + rand()%26);
			i[0] = 12345678;
			ENetPacket* packet = enet_packet_create(i, sizeof(int) * 4, ENET_PACKET_FLAG_UNSEQUENCED);
			enet_peer_send(mHostPeer, 0, packet);
		}else
		{
			printf("Enet client couldn't connect to host\n");
			enet_peer_reset(mHostPeer);
		}
	}
}

void RemoteServerClientBridge::startListening()
{
	mStop = false;
	if(mIsHost)
	{
		createHost();
	}
	mThread = std::thread(&RemoteServerClientBridge::mListenerFunction, this);
}
void RemoteServerClientBridge::stopListening()
{
	mStop = true;
	mThread.join();
}
void RemoteServerClientBridge::mListenerFunction()
{
	ENetEvent event;
	while(!mStop){
		while(enet_host_service(mHost, &event, 10) > 0)
		{
			switch(event.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
					printf ("A new client connected from %x:%u.\n", 
							event.peer -> address.host,
							event.peer -> address.port);
					/* Store any relevant client information here. */
					event.peer -> data = (void*)"Client"; 
					break;
				case ENET_EVENT_TYPE_RECEIVE:
					{
						int pp = ((int*)event.packet->data)[0];
						printf ("A packet of length %u containing %i was received from %s on channel %u.\n",
								event.packet -> dataLength,
								pp,
								event.peer   -> data,
								event.channelID);
						/* Clean up the packet now that we're done using it. */
						enet_packet_destroy (event.packet);
						break;
					}
				case ENET_EVENT_TYPE_DISCONNECT:
					printf ("%s disconnected.\n", event.peer -> data);
					/* Reset the peer's client information. */
					event.peer -> data = NULL;
			}
		}
		enet_host_flush(mHost);
	}
}

void RemoteServerClientBridge::createHost()
{
	//	enet_address_set_host(&mAddress, "localhost");
	mAddress.host = ENET_HOST_ANY;
	mAddress.port = mPort;
	mHost = enet_host_create(&mAddress, //What address to host on.
			32,		//Maximum connections.
			2,		//Maximum channels.
			0,		//Estimated incoming traffic.
			0);		//Estimated outcoming traffic.
	mIsHost = true;
	if(mHost == NULL)
	{
		printf("Server couldn't create, port already in use.\n");
		exit(1);
	}else
	{
		printf("ENet host created!\n");
	}
}

void RemoteServerClientBridge::createClient()
{
	mHost = enet_host_create(NULL, //This isn't going to host anything. noone can connect to this.
			1,//one outgoing connection.
			2,//channels
			0,//out
			0);// no this is out, the one above is incoming.
	if(mHost)
	{
		printf("enet client created!\n");
	}
}
void RemoteServerClientBridge::flush()
{

	enet_host_flush(mHost);
}

void RemoteServerClientBridge::receivePackage(std::weak_ptr<BasePackage> incoming)
{
	mIncoming.push_back(incoming.lock());
}
