#pragma once
#include <stdint.h>
#include <string>
#include <fea/util.hpp>
#include <fea/entitysystem.hpp>
#include "../utilities/glm.hpp"
#include "../world/worlddefines.hpp"

class ScriptEntityCore
{
    public:
        ScriptEntityCore(fea::MessageBus& bus, fea::EntityPtr entity, size_t id);
        void addRef();
        void release();
        void setPosition(const glm::vec3& vec);
        glm::vec3 getPosition();
        bool isOnGround();
        void setId(size_t id);
        WorldId getCurrentWorldId() const;
    private:
        fea::MessageBus& mBus;
        fea::EntityPtr mEntity;
        int32_t mRefCount;
        size_t mId;
};
