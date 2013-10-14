#pragma once
#include "package.h"
#include <vector>
#include <memory>

class ServerClientBridge
{
    public:
        std::unique_ptr<Package> pollPackage(); //fetch a received package
        virtual void flush() = 0; //send all information
        void enqueuePackage(std::unique_ptr<Package> package);
    private:
        std::vector<std::unique_ptr<Package> > incoming;
        std::vector<std::unique_ptr<Package> > outgoing;
};
