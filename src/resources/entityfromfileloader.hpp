#pragma once
#include <string>
#include <fea/entitysystem.hpp>
#include <vector>
#include <utility>

class EntityFromFileLoader
{
    public:
        std::vector<std::pair<std::string, fea::EntityTemplate>> load(const std::string& filename);
};
