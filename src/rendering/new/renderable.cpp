#include "renderable.hpp"

std::type_index Renderable::getType() const
{
    return std::type_index(typeid(Renderable));
}
