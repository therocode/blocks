#pragma once
#include "package.h"
#include <deque>
#include <memory>

class ServerClientBridge
{
    public:
        bool pollPackage(std::shared_ptr<BasePackage>& package); //fetch a received package
        virtual void flush() = 0; //send all information
        void enqueuePackage(std::weak_ptr<BasePackage> package);
    protected:
        std::deque<std::shared_ptr<BasePackage> > mIncoming;
        std::deque<std::shared_ptr<BasePackage> > mOutgoing;
};