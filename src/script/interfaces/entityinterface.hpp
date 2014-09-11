#pragma once
#include "scriptinterface.hpp"
#include "utilities/random.hpp"
#include "../scriptentity.hpp"

class EntityInterface : public ScriptInterface
{
    public:
        EntityInterface(fea::MessageBus& bus, WorldInterface& worldInterface, std::map<size_t, ScriptEntity>& scriptEntities);
        void registerInterface(asIScriptEngine* engine) override;
        asIScriptObject* createEntity(const std::string& type, float x, float y, float z);
        void removeEntityFromId(size_t id);
        void setEntityCreator(EntityCreator creator);
    private:
        EntityCreator mEntityCreator;
        std::map<size_t, ScriptEntity>& mScriptEntities;
};
