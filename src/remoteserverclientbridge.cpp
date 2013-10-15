#include "remoteserverclientbridge.h"
#include <iostream>

bool RemoteServerClientBridge::sEnetInitialized = false;

RemoteServerClientBridge::RemoteServerClientBridge(bool isServer)
{
	mIsHost = false;
	mConnected = false;
	if(!RemoteServerClientBridge::sEnetInitialized)
	{
		enet_initialize();
		RemoteServerClientBridge::sEnetInitialized = true;
	}
	mPort = 6354;
	if(isServer)
	{
		createHost();
	}else
	{
		createClient();
	}
}
void RemoteServerClientBridge::connectToAddress(std::string address)
{
	if(!mIsHost && !mConnected)
	{
		enet_address_set_host(&mAddress, address.c_str());	
		mHostPeer = enet_host_connect(mHost, &mAddress, 2, 0);
		if(mHostPeer == NULL)
		{
			printf("couldn't create connection peer\n");
		}
		else
		{
		}
		ENetEvent event;
		if(enet_host_service(mHost, &event, 5000) > 0 &&
				event.type == ENET_EVENT_TYPE_CONNECT)
		{
			printf("Enet client connected to host!\n");
		}else
		{
			printf("Enet client couldn't connect to host\n");
			enet_peer_reset(mHostPeer);
		}		
	}

}

void RemoteServerClientBridge::startListening()
{
	mThread = std::thread(&RemoteServerClientBridge::mListenerFunction, this);
	mStop = false;
}
void RemoteServerClientBridge::stopListening()
{
	mStop = true;
	mThread.join();
}
void RemoteServerClientBridge::mListenerFunction()
{
	ENetEvent event;
	while(!mStop && enet_host_service(mHost, &event, 1000) > 0)
	{
		switch(event.type)
		{
			case ENET_EVENT_TYPE_CONNECT:
				printf ("A new client connected from %x:%u.\n", 
						event.peer -> address.host,
						event.peer -> address.port);
				/* Store any relevant client information here. */
				//event.peer -> data = "Client information";
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				printf ("A packet of length %u containing %s was received from %s on channel %u.\n",
						event.packet -> dataLength,
						event.packet -> data,
						event.peer -> data,
						event.channelID);
				/* Clean up the packet now that we're done using it. */
				enet_packet_destroy (event.packet);

				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				printf ("%s disconected.\n", event.peer -> data);
				/* Reset the peer's client information. */
				event.peer -> data = NULL;
		}
	}
}

void RemoteServerClientBridge::createHost()
{
	enet_address_set_host(&mAddress, "localhost");
//	mAddress.host = ENET_HOST_ANY;
	mAddress.port = mPort;
	mHost = enet_host_create(&mAddress, //What address to host on.
			32,		//Maximum connections.
			2,		//Maximum channels.
			0,		//Estimated incoming traffic.
			0);		//Estimated outcoming traffic.
	mIsHost = true;
	if(mHost == NULL)
	{
		printf("Server couldn't create\n");
	}else
	{
		printf("ENet host created!\n");
	}
}

void RemoteServerClientBridge::createClient()
{
	mHost = enet_host_create(NULL, //This isn't going to host anything.
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
	for(uint32_t i = 0; i < mOutgoing.size(); i++)
	{
		mOther->receivePackage(std::move(mOutgoing[i]));
	}

	mOutgoing.clear();
}

void RemoteServerClientBridge::connect(RemoteServerClientBridge* other)
{
	mOther = other;
}

void RemoteServerClientBridge::receivePackage(std::unique_ptr<Package>&& incoming)
{
	mIncoming.push_back(std::move(incoming));
}
