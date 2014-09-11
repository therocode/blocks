#pragma once
#include <vector>
#include <memory>
#include <fea/entitysystem.hpp>
#include "controllers/entitycontroller.hpp"
#include "entityfactory.hpp"
#include "entitycreator.hpp"
#include "entitymessages.hpp"
#include "utilities/timer.hpp"

class asIScriptObject;

class EntitySystem : 
    public fea::MessageReceiver<CreateEntityMessage>,
    public fea::MessageReceiver<RemoveEntityMessage>
{
    public:
        EntitySystem(fea::MessageBus& bus);
        ~EntitySystem();
        void addController(std::unique_ptr<EntityController> controller);
        void setup();
        void update();
        void destroy();
        fea::WeakEntityPtr createEntity(const std::string& scriptType, const glm::vec3& position);
        void handleMessage(const CreateEntityMessage& received);
        void handleMessage(const RemoveEntityMessage& received);

        template<class Type>
        Type getEntityAttribute(fea::EntityId id, const std::string& name)
        {
            return mManager.findEntity(id).lock()->getAttribute<Type>(name);
        }
        EntityCreator getEntityCreator();
    private:
        void attachEntity(fea::WeakEntityPtr entity);
        void removeEntity(fea::EntityId id);
        fea::MessageBus& mBus;
        fea::EntityManager mManager;
        EntityFactory mFactory;
        std::vector<std::unique_ptr<EntityController> > mControllers;
        std::string mLogName;
        Timer mTimer;
};
