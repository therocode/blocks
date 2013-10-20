#pragma once
#include "serverclientbridge.h"

class LocalServerClientBridge : public ServerClientBridge
{
    public:
        LocalServerClientBridge();
        virtual void flush() override; //local server client bridge should just serialise and give to the other end
        void connect(LocalServerClientBridge* other); //connect the bridge to another local bridge
        void receivePackage(std::weak_ptr<BasePackage> incoming);
    private:
        LocalServerClientBridge* mOther;
};
