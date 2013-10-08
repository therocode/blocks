#pragma once
#include <featherkit/structure.h>

class BlocksApplication : public fea::Application
{
    public:
        void setup() override;
        void loop() override;
        void destroy() override;
};
