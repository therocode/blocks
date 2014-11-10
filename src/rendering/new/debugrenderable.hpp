#pragma once
#include "renderable.hpp"

class DebugRenderable : public Renderable
{
    virtual std::type_index getType() const;
};
