#pragma once
#include "renderable.hpp"
#include "../utilities/glm.hpp"
#include "../utilities/gimgui.hpp"

class GuiRenderable : public Renderable
{
    public:
        GuiRenderable(const gim::Element& element);
        virtual std::type_index getType() const;

        const gim::Element& element() const;
    private:
        const gim::Element& mElement;
};
