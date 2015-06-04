#pragma once
#include <typeindex>
#include "../utilities/glm.hpp"
#include "rendermode.hpp"
#include "shader.hpp"
#include "texturearray.hpp"
#include "../resources/texturedefinition.hpp"
#include <memory>

class Renderable;
class Camera;

class RenderModule
{
    public:
        RenderModule();
        virtual ~RenderModule();
        void metaQueue(const Renderable& renderable);
        void metaRender(const Camera& camera, const glm::mat4& perspective, const Shader& shader);
        void enableRenderMode(bool enabled);
        virtual std::type_index getRenderableType() const = 0;
        const RenderMode& getRenderMode() const;
        RenderMode& getRenderMode();
        void setRenderMode(const RenderMode& renderMode);
        bool isEnabled() const;
        void setEnabled(bool enabled);
        virtual void textureArrayAdded(uint32_t id, TextureArray& textureArray);
        virtual void textureDefinitionAdded(const std::string& name, const TextureDefinition& textureDefinition);
    protected:
        virtual void render(const Camera& camera, const glm::mat4& perspective, const Shader& shader) = 0;
        virtual void queue(const Renderable& renderable) = 0;
        RenderMode mRenderMode;
        bool mModeEnabled;
        bool mEnabled;
};
