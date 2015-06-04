#pragma once
#include <fea/util.hpp>
#include "../utilities/glm.hpp"
#include "../utilities/gimgui.hpp"
#include "../rendering/renderingmessages.hpp"
#include <fea/rendering/color.hpp>

class DebugUi:
    fea::MessageReceiver<GuiTextureAddedMessage>
{
    public:
        DebugUi(fea::MessageBus& bus);
        const gim::Element& root() const;
        void handleMessage(const GuiTextureAddedMessage& received) override;
    private:
        fea::MessageBus& mBus;
        gim::Element mRoot;
};
