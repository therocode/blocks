#define CATCH_CONFIG_MAIN
#include <thread>
#include "../catch.hpp"

#include "networking/enet.hpp"
#include "networking/enetserver.hpp"
#include "networking/enetclient.hpp"

SCENARIO("ENet can be initialized", "[networking")
{
    GIVEN("An enet instance")
    {
        ENet enet;

        THEN("ENet is initialised")
        {
            CHECK(enet.isInitialized());
        }
    }
}

SCENARIO("Servers can listen to incoming connections", "[networking]")
{
    GIVEN("A listening server")
    {
        ENet enet;
        ENetServer server(enet, 56556);
        
        THEN("The server listens for incoming connections")
        {
            CHECK(server.isListening());
        }
    }
}

SCENARIO("Servers and clients can connect", "[networking]")
{
    GIVEN("A server and two clients")
    {
        ENet enet;
        ENetServer server(enet, 56556);
        ENetClient client1(enet);
        ENetClient client2(enet);

        WHEN("Nothing is done")
        {
            THEN("They are not connected")
            {
                CHECK(server.getClientCount() == 0);
                CHECK_FALSE(client1.isConnected());
                CHECK_FALSE(client2.isConnected());
            }
        }

        WHEN("Clients connect to the server")
        {
            std::thread connector1([&] () {for(int32_t i = 0; i < 5; i++) client1.connect("localhost", 56556, 100);});
            server.update(100);
            connector1.join();

            THEN("They are connected")
            {
                CHECK(server.getClientCount() == 1);
                CHECK(client1.isConnected());
            }

            std::thread connector2([&] () {for(int32_t i = 0; i < 5; i++) client2.connect("localhost", 56556, 100);});
            server.update(100);
            connector2.join();

            THEN("They are connected")
            {
                CHECK(server.getClientCount() == 2);
                CHECK(client2.isConnected());
            }
        }

        WHEN("Clients connect to the wrong port or address")
        {
            std::thread connector([&] () {for(int32_t i = 0; i < 100; i++) {client1.connect("pallkars.net", 56556, 10); client2.connect("127.0.0.1", 65665, 10);}});
            server.update(50);
            server.update(50);
            connector.join();

            THEN("They are not connected")
            {
                CHECK(server.getClientCount() == 0);
                CHECK_FALSE(client1.isConnected());
                CHECK_FALSE(client2.isConnected());
            }
        }
    }
}

SCENARIO("Servers and clients can disconnect", "[networking]")
{
    GIVEN("A client connected to a server")
    {
        ENet enet;
        ENetServer server(enet, 56556);
        ENetClient client(enet);

        std::thread connector(&ENetServer::update, &server, 100);
        client.connect("localhost", 56556, 100);
        connector.join();
        server.update(100);

        WHEN("Clients disconnect from a server")
        {
            client.disconnect();
            server.update(100);

            THEN("Clients are properly disconnected")
            {
                CHECK(server.getClientCount() == 0);
                CHECK_FALSE(client.isConnected());
            }
        }
    }
}
