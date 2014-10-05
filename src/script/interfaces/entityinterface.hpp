#pragma once
#include "scriptinterface.hpp"
#include "../../utilities/random.hpp"
#include "../scriptentity.hpp"
#include <unordered_map>
#include "../../entity/entitymessages.hpp"

class EntityInterface : 
    public ScriptInterface,
    public fea::MessageReceiver<EntityCreatedMessage>
{
    public:
        EntityInterface(fea::MessageBus& bus, GameInterface& worldInterface, std::unordered_map<size_t, ScriptEntity>& scriptEntities);
        void registerInterface(asIScriptEngine* engine) override;
        asIScriptObject* createEntity(const std::string& type, WorldId worldId, const  glm::vec3 position);
        void removeEntityFromId(size_t id);
        void handleMessage(const EntityCreatedMessage& message);
    private:
        fea::MessageBus& mBus;
        std::unordered_map<size_t, ScriptEntity>& mScriptEntities;
        fea::WeakEntityPtr mNewlyCreated;
};
