#pragma once
#include <vector>
#include <memory>
#include <fea/entitysystem.hpp>
#include "entityfactory.hpp"
#include "entitymessages.hpp"
#include "../utilities/timer.hpp"
#include "../resources/resourcemessages.hpp"

class EntityController;

class EntitySystem : 
    public fea::MessageReceiver<EntityRequestedMessage, RemoveEntityRequestedMessage, ResourceDeliverMessage<std::unordered_map<std::string, std::string>>, ResourceDeliverMessage<std::vector<std::pair<std::string, fea::EntityTemplate>>>>
{
    public:
        EntitySystem(fea::MessageBus& bus);
        ~EntitySystem();
        void addController(std::unique_ptr<fea::EntityComponent> controller);
        void handleMessage(const EntityRequestedMessage& received);
        void handleMessage(const RemoveEntityRequestedMessage& received);
        const fea::EntityManager& getEntityManager() const;
		void handleMessage(const ResourceDeliverMessage<std::unordered_map<std::string, std::string>>& received);
		void handleMessage(const ResourceDeliverMessage<std::vector<std::pair<std::string, fea::EntityTemplate>>>& received);
    private:
        fea::WeakEntityPtr createEntity(const std::string& scriptType, std::function<void(fea::EntityPtr)> initializer);
        void attachEntity(fea::WeakEntityPtr entity);
        void removeEntity(fea::EntityId id);
        fea::MessageBus& mBus;
        fea::EntityManager mManager;
        EntityFactory mFactory;
        std::vector<std::unique_ptr<fea::EntityComponent>> mControllers;
};
