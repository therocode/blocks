#pragma once
#include "world.h"

class Server
{
    public:
        Server();
        void setup();
        void doLogic();
        void destroy();
    private:
        fea::MessageBus mBus;
        World world;
};
