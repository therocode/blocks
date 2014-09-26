#pragma once
#include <vector>
#include <memory>
#include <fea/entitysystem.hpp>
#include "entityfactory.hpp"
#include "entitymessages.hpp"
#include "utilities/timer.hpp"

class EntityController;

class EntitySystem : 
    public fea::MessageReceiver<EntityRequestedMessage, RemoveEntityRequestedMessage>
{
    public:
        EntitySystem(fea::MessageBus& bus);
        ~EntitySystem();
        void addController(std::unique_ptr<EntityController> controller);
        void setup();
        void update(int32_t deltaTime);
        void handleMessage(const EntityRequestedMessage& received);
        void handleMessage(const RemoveEntityRequestedMessage& received);
        const fea::EntityManager& getEntityManager() const;
    private:
        fea::WeakEntityPtr createEntity(const std::string& scriptType, std::function<void(fea::EntityPtr)> initializer);
        void attachEntity(fea::WeakEntityPtr entity);
        void removeEntity(fea::EntityId id);
        fea::MessageBus& mBus;
        fea::EntityManager mManager;
        EntityFactory mFactory;
        std::vector<std::unique_ptr<EntityController>> mControllers;
};
