#pragma once
#include <vector>
#include <memory>
#include <fea/entitysystem.hpp>
#include "entityfactory.hpp"
#include "entitymessages.hpp"
#include "utilities/timer.hpp"

class EntityController;
class asIScriptObject;

class EntitySystem : 
    public fea::MessageReceiver<EntityRequestedMessage, RemoveEntityMessage>
{
    public:
        EntitySystem(fea::MessageBus& bus);
        ~EntitySystem();
        void addController(std::unique_ptr<EntityController> controller);
        void setup();
        void update();
        fea::WeakEntityPtr createEntity(const std::string& scriptType, std::function<void(fea::EntityPtr)> initializer);
        void handleMessage(const EntityRequestedMessage& received);
        void handleMessage(const RemoveEntityMessage& received);

        template<class Type>
        Type getEntityAttribute(fea::EntityId id, const std::string& name)
        {
            return mManager.findEntity(id).lock()->getAttribute<Type>(name);
        }
    private:
        void attachEntity(fea::WeakEntityPtr entity);
        void removeEntity(fea::EntityId id);
        fea::MessageBus& mBus;
        fea::EntityManager mManager;
        EntityFactory mFactory;
        std::vector<std::unique_ptr<EntityController> > mControllers;
        Timer mTimer;
};
