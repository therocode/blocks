#include "guirenderable.hpp"

GuiRenderable::GuiRenderable(const gim::Element& element):
    mElement(element)
{
}

std::type_index GuiRenderable::getType() const
{
    return std::type_index(typeid(GuiRenderable));
}

const gim::Element& GuiRenderable::element() const
{
    return mElement;
}
