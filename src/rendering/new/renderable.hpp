#pragma once
#include <typeindex>

class Renderable
{
    public:
        virtual std::type_index getType() const;
};
