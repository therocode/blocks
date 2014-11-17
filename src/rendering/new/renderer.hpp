#pragma once
#include "glcontext.hpp"
#include "rendermodule.hpp"
#include "camera.hpp"
#include <memory>
#include <vector>
#include <typeindex>
#include <unordered_set>
#include <unordered_map>

class Renderable;

class Renderer
{
    public:
        Renderer(const GLContext& glContext);
        void addModule(std::unique_ptr<RenderModule> module);
        void queue(const Renderable& renderable);
        void render();
        const Camera& getCamera() const;
        Camera& getCamera();
        void setCamera(const Camera& camera);
    private:
        std::unordered_set<std::unique_ptr<RenderModule>> mModules;
        std::unordered_map<std::type_index, std::vector<RenderModule*>> mModuleSubscriptions;
        Camera mCamera;
};
