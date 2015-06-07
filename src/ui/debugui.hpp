#pragma once
#include <fea/util.hpp>
#include "../utilities/glm.hpp"
#include "../utilities/gimgui.hpp"
#include "../rendering/renderingmessages.hpp"
#include <fea/rendering/color.hpp>

class DebugUi:
    public fea::MessageReceiver<GuiTextureAddedMessage, GuiFontAddedMessage>
{
    public:
        DebugUi(fea::MessageBus& bus);
        const gim::Element& root() const;
        void handleMessage(const GuiTextureAddedMessage& received) override;
        void handleMessage(const GuiFontAddedMessage& received) override;
    private:
        fea::MessageBus& mBus;
        gim::Element mRoot;
};
