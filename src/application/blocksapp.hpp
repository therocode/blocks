#pragma once
#include "../utilities/glm.hpp"
#include "../networking/networkingmessages.hpp"
#include <fea/structure.hpp>
#include <fea/util.hpp>
#include "client.hpp"
#include "server.hpp"

class BlocksApplication 
    :   public fea::Application,
        public fea::MessageReceiver<LocalConnectionAttemptMessage>
{
    public:
        BlocksApplication();
        void setup(const std::vector<std::string>& args) override;
        void loop() override;
        void destroy() override;
        void handleMessage(const LocalConnectionAttemptMessage& received) override;
    private:
		void setupMultiPlayer(int32_t port);
        void setupSinglePlayer();
        void setupDedicatedServer(int32_t port);
        void joinServer(const std::string& address, int32_t port);
        fea::MessageBus mServerBus;
        fea::MessageBus mClientBus;
        std::unique_ptr<Server> server;  //server instance. Will be null in the case of connecting to a remote server.
        std::unique_ptr<Client> client;  //local client instance. in the case of a headless dedicated server, this will be null.
};
