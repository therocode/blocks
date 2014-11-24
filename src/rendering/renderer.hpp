#pragma once
#include "glcontext.hpp"
#include "rendermodule.hpp"
#include "camera.hpp"
#include "rendermode.hpp"
#include <memory>
#include <vector>
#include <typeindex>
#include <map>
#include <unordered_map>

class Renderable;

class Renderer
{
    public:
        Renderer(const GLContext& glContext, const glm::uvec2& viewSize);
        void addModule(int32_t id, std::unique_ptr<RenderModule> module);
        void queue(const Renderable& renderable);
        void render();
        const Camera& getCamera() const;
        Camera& getCamera();
        void setCamera(const Camera& camera);
        void setViewSize(const glm::uvec2& size);
        const RenderMode& getRenderMode() const;
        RenderMode& getRenderMode();
        void setRenderMode(const RenderMode& renderMode);
        const RenderMode* findRenderMode(int32_t moduleId) const;
        RenderMode* findRenderMode(int32_t moduleId);
        void setRenderMode(int32_t moduleId, const RenderMode& renderMode);
        bool isEnabled() const;
        void setEnabled(bool enabled);
        bool isEnabled(int32_t moduleId) const;
        void setEnabled(int32_t moduleId, bool enabled);
        void setPerspective(float fov, float near, float far);
    private:
        std::map<int32_t, std::unique_ptr<RenderModule>> mModules;
        std::unordered_map<std::type_index, std::vector<RenderModule*>> mModuleSubscriptions;
        Camera mCamera;
        glm::uvec2 mViewSize;
        float mFov;
        float mNear;
        float mFar;
        RenderMode mRenderMode;
        bool mEnabled;
        glm::mat4 mPerspective;
};
