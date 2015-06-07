#include "debugui.hpp"
#include <fstream>
#include "../resources/texturedefinition.hpp"

using namespace std::literals::string_literals;
        
DebugUi::DebugUi(fea::MessageBus& bus):
    mBus(bus),
    mLoadedChunkAmount(0)
{
    subscribe(mBus, *this);
    //GimTextureAdaptor textureAdaptor;
    //gim::RenderDataGenerator<Vec2, Color> generator;

    //uint32_t textureId = generator.registerFontStorage({font}, textureAdaptor);

    mRoot = gim::Element({"text"},
    {
        {"position", glm::ivec2({0, 0})},
        {"size", glm::ivec2({600, 400})},
        {"color", fea::Color(55, 255, 255, 0)},
        {"z_position", -9.0f},
        {"text", "This is a text"s},
        {"text_size", 16},
        {"text_borders", Rectangle({{8, 8}, {600 - 16, 400 - 16}})},
        {"text_z_position", -8.0f},
        {"text_color", fea::Color::Black},
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

void DebugUi::handleMessage(const GuiFontAddedMessage& received)
{
    if(!mRoot.hasAttribute<std::string>("font"))
    {
        mRoot.createAttribute("font", received.font.name());
    }
}

void DebugUi::handleMessage(const LoadedChunkAmountMessage& received)
{
    mLoadedChunkAmount = received.amount;

    update();
}

void DebugUi::update()
{
    std::string text = "Loaded chunks: " + std::to_string(mLoadedChunkAmount);
    mRoot.setAttribute("text", text);
}
