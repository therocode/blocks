#pragma once
#include <vector>
#include <memory>
#include <featherkit/entitysystem.h>
#include "controllers/entitycontroller.h"
#include "entityfactory.h"
#include "entitymessages.h"

class asIScriptObject;

class EntitySystem : 
    public fea::MessageReceiver<SpawnEntityMessage>,
    public fea::MessageReceiver<RemoveEntityMessage>
{
    public:
        EntitySystem(fea::MessageBus& bus);
        ~EntitySystem();
        void addController(std::unique_ptr<EntityController> controller);
        void update();
        fea::WeakEntityPtr spawnEntity(const std::string& scriptType, const glm::vec3& position);
        size_t spawnEntityFromScriptHandle(const std::string& scriptType, const glm::vec3& position, asIScriptObject* obj);
        void handleMessage(const SpawnEntityMessage& received);
        void handleMessage(const RemoveEntityMessage& received);

        template<class Type>
        Type getEntityAttribute(fea::EntityId id, const std::string& name)
        {
            return mManager.getEntity(id).lock()->getAttribute<Type>(name);
        }
    private:
        void attachEntity(fea::WeakEntityPtr entity);
        void removeEntity(fea::EntityId id);
        fea::MessageBus& mBus;
        fea::EntityManager mManager;
        EntityFactory mFactory;
        std::vector<std::unique_ptr<EntityController> > mControllers;
        std::string mLogName;
};
