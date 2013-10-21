#pragma once
#include <stdint.h>
#include <string>
#include <featherkit/messaging.h>

class ScriptEntityCore
{
    public:
        ScriptEntityCore(fea::MessageBus& bus, size_t id);
        void addRef();
        void release();
        void setPosition(float x, float y, float z);
        void setId(size_t id);
    private:
        fea::MessageBus& mBus;
        int32_t mRefCount;
        size_t mId;
};
