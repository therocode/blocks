#pragma once
#include <vector>
#include <memory>
#include <featherkit/entitysystem.h>
#include "controllers/entitycontroller.h"
#include "entityfactory.h"
#include "messages.h"

class asIScriptObject;

class EntitySystem : 
    public fea::MessageReceiver<SpawnEntityMessage>
{
    public:
        EntitySystem(fea::MessageBus& bus);
        ~EntitySystem();
        void addController(std::unique_ptr<EntityController> controller);
        void update();
        fea::WeakEntityPtr spawnEntity(const std::string& scriptType, const glm::vec3& position);
        void spawnEntityFromScriptHandle(const std::string& scriptType, const glm::vec3& position, asIScriptObject* obj);
        void handleMessage(const SpawnEntityMessage& received);
    private:
        void attachEntity(fea::WeakEntityPtr entity);
        fea::MessageBus& mBus;
        fea::EntityManager mManager;
        EntityFactory mFactory;
        std::vector<std::unique_ptr<EntityController> > mControllers;
};



//todo:
//entity desctruction:
//destroy entity message sent with id
//catch by entity system
//entity system informs others with entity deleted message
//entity system removes from controllers
//entity system removes from manager
