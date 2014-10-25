#pragma once
#include "../utilities/glm.hpp"
#include "../networking/networkingmessages.hpp"
#include <fea/structure.hpp>
#include <fea/util.hpp>
#include "client.hpp"

#if !defined(CLIENT_ONLY)
#include "server.hpp"
#endif

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
#if !defined(CLIENT_ONLY)
        fea::MessageBus mServerBus;
#endif
        fea::MessageBus mClientBus;
#if !defined(CLIENT_ONLY)
        std::unique_ptr<Server> server;  //server instance. Will be null in the case of connecting to a remote server.
#endif
        std::unique_ptr<Client> client;  //local client instance. in the case of a headless dedicated server, this will be null.
};
