#pragma once
#include <stdint.h>
#include <string>
#include <featherkit/messaging.h>
#include "../world/worldinterface.h"

class ScriptEntityCore
{
    public:
        ScriptEntityCore(size_t id);
        void addRef();
        void release();
        void setPosition(float x, float y, float z);
        void setId(size_t id);
        static WorldInterface* sWorldInterface;
        static fea::MessageBus* sBus;
    private:
        int32_t mRefCount;
        size_t mId;
};
