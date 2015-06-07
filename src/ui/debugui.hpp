#pragma once
#include <fea/util.hpp>
#include "../utilities/glm.hpp"
#include "../utilities/gimgui.hpp"
#include "../rendering/renderingmessages.hpp"
#include <fea/rendering/color.hpp>
#include "uimessages.hpp"

class DebugUi: public fea::MessageReceiver<
               GuiTextureAddedMessage,
               GuiFontAddedMessage,
               LoadedChunkAmountMessage>
{
    public:
        DebugUi(fea::MessageBus& bus);
        const gim::Element& root() const;
        void handleMessage(const GuiTextureAddedMessage& received) override;
        void handleMessage(const GuiFontAddedMessage& received) override;
        void handleMessage(const LoadedChunkAmountMessage& received) override;
    private:
        void update();
        fea::MessageBus& mBus;
        gim::Element mRoot;

        uint32_t mLoadedChunkAmount;
};
