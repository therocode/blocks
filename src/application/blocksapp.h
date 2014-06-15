#pragma once
#include "../blockstd.h"
#include <fea/structure.hpp>
#include "client.h"
#include "server.h"

class BlocksApplication 
    :   public fea::Application
{
    public:
        BlocksApplication();
        void setup(const std::vector<std::string>& args) override;
        void loop() override;
        void destroy() override;
    private:
		void setupMultiPlayer();
        void setupSinglePlayer();
        void setupDedicatedServer(int32_t port);
        void joinServer(const std::string& address, int32_t port);
        std::unique_ptr<Client> client;  //local client instance. in the case of a headless dedicated server, this will be null.
        std::unique_ptr<Server> server;  //server instance. Will be null in the case of connecting to a remote server.
};
