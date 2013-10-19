#pragma once
#include <featherkit/entitysystem.h>

class EntityFactory
{
    public:
        EntityFactory(fea::EntityManager& manager);
        fea::WeakEntityPtr spawnEntity(const std::string& scriptType);
    private:
        fea::EntityManager& mManager;
};
