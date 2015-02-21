#pragma once
#include <fea/entitysystem.hpp>
#include "entitydefinition.hpp"

class EntityFactory
{
    public:
        EntityFactory(fea::EntityManager& manager);
        fea::WeakEntityPtr spawnEntity(const std::string& scriptType);
        void addDefinition(const EntityDefinition& definition);
		void registerAttribute(const std::unordered_map<std::string, std::string> &attribute);
		void registerEntity(const std::vector<std::pair<std::string, fea::EntityTemplate>> &entity);
    private:
        fea::EntityFactory mFactory;
        std::unordered_map<std::string, EntityDefinition> mEntityDefinitions;
};
