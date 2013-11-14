#include "remoteserverbridge.h"
#include <iostream>
#include "packages.h"
#include "application/applicationmessages.h"

RemoteServerBridge::RemoteServerBridge(fea::MessageBus& bus) : mBus(bus), mLogName("network")
{
	mConnected = false;

	mPort = 35940;
}

void RemoteServerBridge::connectToAddress(std::string address, int port)
{
	mBus.sendMessage<LogMessage>(LogMessage("Connecting to " + address, mLogName, LogLevel::INFO));
	if(!mConnected)
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
			mBus.sendMessage<LogMessage>(LogMessage("Couldn't create connection peer", mLogName, LogLevel::ERR));
		}

		ENetEvent event;
		if(enet_host_service(mHost, &event, 5000) > 0 &&
			event.type == ENET_EVENT_TYPE_CONNECT)
		{
			mBus.sendMessage<LogMessage>(LogMessage("Successfully connected to " + address, mLogName, LogLevel::INFO));
			//When connected, greet the server!
			int i[4];
			for(int o = 1; o < 4; o++)i[o] = (int)(64 + rand()%26);
				i[0] = 12345678;
			ENetPacket* packet = enet_packet_create(i, sizeof(int) * 4, ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(mHostPeer, 0, packet);
		}else
		{
			mBus.sendMessage<LogMessage>(LogMessage("Couldn't connect to host", mLogName, LogLevel::ERR));
			enet_peer_reset(mHostPeer);
		}
	}
}

void RemoteServerBridge::startListening()
{
	mStop = false;

	mThread = std::thread(&RemoteServerBridge::mListenerFunction, this);
}

void RemoteServerBridge::stopListening()
{
	mStop = true;
	mThread.join();
}

void RemoteServerBridge::mListenerFunction()
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
					ENetPacket* packet = event.packet;
					int pp = ((int*)event.packet->data)[0];
					PackageType type;
					printf("Data length %u\n", packet->dataLength);
					uint8_t* typePointer = (uint8_t*)&type;
					for(uint32_t i = 0; i < sizeof(PackageType); i++)
					{
						*typePointer = packet->data[i];
						typePointer++;
					}

					std::vector<uint8_t> dataVector;
					dataVector.resize(packet->dataLength);
					std::copy(packet->data, packet->data + packet->dataLength, dataVector.begin());


					switch(type)
					{
						case PackageType::REBUILD_SCRIPTS_REQUESTED:
						{
							deserialiseAndReceive(dataVector, new RebuildScriptsRequestedPackage());
							break;
						}
						case PackageType::PLAYER_ACTION:
						{
							deserialiseAndReceive(dataVector, new PlayerActionPackage());
							break;
						}
						case PackageType::PLAYER_PITCH_YAW:
						{
							deserialiseAndReceive(dataVector, new PlayerPitchYawPackage());
							break;
						}
						case PackageType::PLAYER_MOVE_ACTION:
						{
							deserialiseAndReceive(dataVector, new PlayerMoveActionPackage());
							break;
						}
						case PackageType::PLAYER_MOVE_DIRECTION:
						{
							deserialiseAndReceive(dataVector, new PlayerMoveDirectionPackage());
							break;
						}
					}
					//	printf ("A packet of length %u containing %i was received from %s on channel %u.\n",
					//			event.packet -> dataLength,
					//			pp,
					//			event.peer   -> data,
					//			event.channelID);
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

void RemoteServerBridge::createClient()
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

void RemoteServerBridge::flush()
{
	for(auto package : mOutgoing)
	{
		std::vector<uint8_t> data = package->serialise();

		ENetPacket* packet = enet_packet_create(&data[0], data.size(), ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(mHostPeer, 0, packet);
	}
	enet_host_flush(mHost);
	mOutgoing.clear();
}

void RemoteServerBridge::receivePackage(std::weak_ptr<BasePackage> incoming)
{
	mIncoming.push_back(incoming.lock());
}
void RemoteServerBridge::deserialiseAndReceive(const std::vector<uint8_t>& data, BasePackage* package)
{
    package->deserialise(data);
    std::shared_ptr<BasePackage> packagePtr = std::shared_ptr<BasePackage>(package);
    receivePackage(packagePtr);
}
