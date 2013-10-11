#include "defaultsetters.h"

void vec3Setter(std::string attribute, std::vector<std::string>& arguments, fea::WeakEntityPtr entity)
{
    entity.lock()->setAttribute<glm::vec3>(attribute, glm::vec3(std::stof(arguments[0]), std::stof(arguments[1]), std::stof(arguments[2])));
}
