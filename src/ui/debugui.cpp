#include "debugui.hpp"
#include <fstream>
#include "../resources/texturedefinition.hpp"

using namespace std::literals::string_literals;
        
DebugUi::DebugUi(fea::MessageBus& bus):
    mBus(bus)
{
    subscribe(mBus, *this);
    //GimTextureAdaptor textureAdaptor;
    //gim::RenderDataGenerator<Vec2, Color> generator;

    //uint32_t textureId = generator.registerFontStorage({font}, textureAdaptor);

    mRoot = gim::Element({"text"},
    {
        {"position", glm::ivec2({10, 20})},
        {"size",     glm::ivec2({30, 40})},
        //{"text", "This is a text"s},
        //{"text_size", 16},
        //{"text_borders", Rectangle({{5, 15}, {25, 35}})},
        //{"font", gim::makeRef(font)},
    });
}

const gim::Element& DebugUi::root() const
{
    return mRoot;
}

void DebugUi::handleMessage(const GuiTextureAddedMessage& received)
{
    if(received.name == "test.textures.borders")
    {
        mRoot.createAttribute("image_id", received.id);
        mRoot.createAttribute("image_coords", Rectangle({{8, 8}, {48, 48}}));
        mRoot.createAttribute("border_mode", gim::BorderMode::FULL);
        mRoot.createAttribute("border_coords_tl", Rectangle({{0 ,0 }, {8 ,8 }}));
        mRoot.createAttribute("border_coords_t",  Rectangle({{8 ,0 }, {48,8 }}));
        mRoot.createAttribute("border_coords_tr", Rectangle({{56,0 }, {8 ,8 }}));
        mRoot.createAttribute("border_coords_r",  Rectangle({{56,8 }, {8 ,48}}));
        mRoot.createAttribute("border_coords_br", Rectangle({{56,56}, {8 ,8 }}));
        mRoot.createAttribute("border_coords_b",  Rectangle({{8 ,56}, {48,8 }}));
        mRoot.createAttribute("border_coords_bl", Rectangle({{0 ,56}, {8 ,8 }}));
        mRoot.createAttribute("border_coords_l",  Rectangle({{0 ,8 }, {8 ,48}}));
    }
}
