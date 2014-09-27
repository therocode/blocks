#pragma once
#include <stdint.h>
#include <string>
#include <fea/util.hpp>
#include "../blockstd.hpp"

class GameInterface;

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
        static GameInterface* sGameInterface;
        static fea::MessageBus* sBus;
    private:
        int32_t mRefCount;
        size_t mId;
};
