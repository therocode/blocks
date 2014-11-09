#pragma once
#include <fea/util.hpp>

class RenderingSystem
{
    public:
        RenderingSystem(fea::MessageBus& bus);
        void render();
    private:
        fea::MessageBus& mBus;
};
