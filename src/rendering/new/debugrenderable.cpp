#include "debugrenderable.hpp"

std::type_index DebugRenderable::getType() const
{
    return std::type_index(typeid(DebugRenderable));
}
