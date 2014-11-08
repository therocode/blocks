#pragma once

class Renderer
{
    public:
        Renderer(const GLContext& glContext);
        void addModule(std::unique_ptr<RenderModule> module);
        void queue(const Renderable& renderable);
        void render();
    private:
        std::vector<std::unique_ptr<RenderModule>> mModules;
};
