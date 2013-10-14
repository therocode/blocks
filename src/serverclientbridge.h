#pragma once
#include "package.h"
#include <deque>
#include <memory>

class ServerClientBridge
{
    public:
        bool pollPackage(std::unique_ptr<Package>& package); //fetch a received package
        virtual void flush() = 0; //send all information
        void enqueuePackage(std::unique_ptr<Package>&& package);
    protected:
        std::deque<std::unique_ptr<Package> > mIncoming;
        std::deque<std::unique_ptr<Package> > mOutgoing;
};
