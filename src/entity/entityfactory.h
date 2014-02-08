#pragma once
#include <featherkit/entitysystem.h>
#include "entitydefinition.h"

class EntityFactory
{
    public:
        EntityFactory(fea::EntityManager& manager);
        fea::WeakEntityPtr spawnEntity(const std::string& scriptType);
        void addDefinition(const EntityDefinition& definition);
    private:
        fea::EntityManager& mManager;
        fea::EntityFactory mFactory;
        std::unordered_map<std::string, EntityDefinition> mEntityDefinitions;
};
