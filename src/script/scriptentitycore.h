#pragma once
#include <stdint.h>
#include <string>
#include <fea/messaging.h>
#include "../world/worldinterface.h"

class ScriptEntityCore
{
    public:
        ScriptEntityCore(size_t id);
        void addRef();
        void release();
        void setPosition(float x, float y, float z);
        void setPosition(const glm::vec3& vec);
        glm::vec3 getPosition();
        bool isOnGround();
        void setId(size_t id);
        static WorldInterface* sWorldInterface;
        static fea::MessageBus* sBus;
    private:
        int32_t mRefCount;
        size_t mId;
};
