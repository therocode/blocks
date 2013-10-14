#pragma once
#include "blockstd.h"
#include <featherkit/structure.h>
#include "client.h"
#include "server.h"

class BlocksApplication 
    :   public fea::Application
{
    public:
        BlocksApplication();
        void setup() override;
        void loop() override;
        void destroy() override;
    private:
        std::unique_ptr<Client> client;  //local client instance. in the case of a headless dedicated server, this will be null.
        std::unique_ptr<Server> server;  //server instance. Will never be null. It can either be a local server or a networked server.
};
