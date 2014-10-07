#pragma once
#include "../utilities/glm.hpp"
#include <fea/structure.hpp>
#include "client.hpp"
#include "server.hpp"

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
        fea::MessageBus mServerBus;
        fea::MessageBus mClientBus;
        std::unique_ptr<Server> server;  //server instance. Will be null in the case of connecting to a remote server.
        std::unique_ptr<Client> client;  //local client instance. in the case of a headless dedicated server, this will be null.
};
