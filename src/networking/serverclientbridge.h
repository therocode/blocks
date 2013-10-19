#pragma once
#include "package.h"
#include <deque>
#include <memory>

class ServerClientBridge
{
    public:
        bool pollPackage(std::unique_ptr<BasePackage>& package); //fetch a received package
        virtual void flush() = 0; //send all information
        void enqueuePackage(std::unique_ptr<BasePackage>&& package);
    protected:
        std::deque<std::unique_ptr<BasePackage> > mIncoming;
        std::deque<std::unique_ptr<BasePackage> > mOutgoing;
};
