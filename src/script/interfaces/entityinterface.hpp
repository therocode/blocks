#pragma once
#include "scriptinterface.hpp"
#include "utilities/random.hpp"
#include "../scriptentity.hpp"
#include "../../entity/entitycreator.hpp"
#include <unordered_map>

class EntityInterface : public ScriptInterface
{
    public:
        EntityInterface(fea::MessageBus& bus, WorldInterface& worldInterface, std::unordered_map<size_t, ScriptEntity>& scriptEntities);
        void registerInterface(asIScriptEngine* engine) override;
        asIScriptObject* createEntity(const std::string& type, float x, float y, float z);
        void removeEntityFromId(size_t id);
        void setEntityCreator(EntityCreator creator);
    private:
        EntityCreator mEntityCreator;
        std::unordered_map<size_t, ScriptEntity>& mScriptEntities;
};
