#pragma once
#include "opengl.hpp"

enum PolygonMode { FILL, LINE, POINT };

class RenderMode
{
    public:
        RenderMode();
        PolygonMode getPolygonMode() const;
        void setPolygonMode(PolygonMode polygonMode);
        void activate() const;
        void deactivate() const;
    private:
        PolygonMode mPolygonMode;
};
