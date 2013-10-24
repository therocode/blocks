#pragma once
#include <featherkit/entitysystem.h>
#include "entitydefinition.h"

class EntityFactory
{
    public:
        EntityFactory(fea::EntityManager& manager);
        fea::WeakEntityPtr spawnEntity(const std::string& scriptType);
    private:
        fea::EntityManager& mManager;
        std::unordered_map<std::string, EntityDefinition> mEntityDefinitions;
};
