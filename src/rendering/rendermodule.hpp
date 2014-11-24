#pragma once
#include <typeindex>
#include "../utilities/glm.hpp"
#include "rendermode.hpp"
#include <memory>

class Renderable;
class Camera;

class RenderModule
{
    public:
        RenderModule();
        virtual ~RenderModule();
        void metaRender(const Camera& camera, const glm::mat4& perspective);
        virtual void queue(const Renderable& renderable) = 0;
        virtual std::type_index getRenderableType() const = 0;
        const RenderMode* findRenderMode() const;
        RenderMode* findRenderMode();
        void setRenderMode(const RenderMode& renderMode);
        bool isEnabled() const;
        void setEnabled(bool enabled);
    protected:
        virtual void render(const Camera& camera, const glm::mat4& perspective) = 0;
        std::unique_ptr<RenderMode> mRenderMode;
        bool mEnabled;
};
