#include "renderingsystem.hpp"
#include "modelrenderer.hpp"
#include "debugrenderer.hpp"

RenderingSystem::RenderingSystem(fea::MessageBus& bus, const glm::uvec2& viewSize) :
    mBus(bus),
    mRenderer(mGLContext, viewSize)

{
    subscribe(bus, *this);
    mRenderer.addModule(RenderModule::MODEL, std::unique_ptr<ModelRenderer>(new ModelRenderer()));
    mRenderer.addModule(RenderModule::DEBUG, std::unique_ptr<DebugRenderer>(new DebugRenderer()));

    std::vector<float> squareVertices = {
             -0.5f, -0.5f,  0.5f,
              0.5f, -0.5f,  0.5f,
             -0.5f,  0.5f,  0.5f,

             -0.5f,  0.5f,  0.5f,
              0.5f, -0.5f,  0.5f,
              0.5f,  0.5f,  0.5f,


             -0.5f, -0.5f, -0.5f,
             -0.5f,  0.5f, -0.5f,
              0.5f, -0.5f, -0.5f,

             -0.5f,  0.5f, -0.5f,
              0.5f,  0.5f, -0.5f,
              0.5f, -0.5f, -0.5f,



             -0.5f, -0.5f,  0.5f,
             -0.5f,  0.5f,  0.5f,
             -0.5f, -0.5f, -0.5f,

             -0.5f, -0.5f, -0.5f,
             -0.5f,  0.5f,  0.5f,
             -0.5f,  0.5f, -0.5f,


              0.5f, -0.5f,  0.5f,
              0.5f, -0.5f, -0.5f,
              0.5f,  0.5f,  0.5f,

              0.5f, -0.5f, -0.5f,
              0.5f,  0.5f, -0.5f,
              0.5f,  0.5f,  0.5f,



             -0.5f,  0.5f,  0.5f,
              0.5f,  0.5f,  0.5f,
             -0.5f,  0.5f, -0.5f,

              0.5f,  0.5f,  0.5f,
              0.5f,  0.5f, -0.5f,
             -0.5f,  0.5f, -0.5f,


             -0.5f, -0.5f,  0.5f,
             -0.5f, -0.5f, -0.5f,
              0.5f, -0.5f,  0.5f,

              0.5f, -0.5f,  0.5f,
             -0.5f, -0.5f, -0.5f,
              0.5f, -0.5f, -0.5f,
    };

    std::vector<float> tetraVertices = {
              0.0f,  1.0f,  0.0f,
             -0.5f,  0.0f,  0.5f,
              0.5f,  0.0f,  0.5f,

              0.0f,  1.0f,  0.0f,
             -0.5f,  0.0f, -0.5f,
             -0.5f,  0.0f,  0.5f,

              0.0f,  1.0f,  0.0f,
              0.5f,  0.0f, -0.5f,
             -0.5f,  0.0f, -0.5f,

              0.0f,  1.0f,  0.0f,
              0.5f,  0.0f, -0.5f,
              0.5f,  0.0f,  0.5f
    };

    mCubeMesh = std::unique_ptr<Mesh>(new Mesh(squareVertices));
    mTetraMesh = std::unique_ptr<Mesh>(new Mesh(tetraVertices));

    mCubeModel.addMesh(0, *mCubeMesh);
    mTetraModel.addMesh(0, *mTetraMesh);
    mCubeModelRenderable.setModel(mCubeModel);
    mTetraModelRenderable.setModel(mTetraModel);

    for(uint32_t x = 0; x < 50; x++)
    {
        for(uint32_t y = 0; y < 50; y++)
        {
            for(uint32_t z = 0; z < 50; z++)
            {
                DebugRenderable newDeb;
                newDeb.setPosition(glm::vec3(x * 5 + (float)(rand() % 20 - 10) / 5.0f, y * 5 + (float)(rand() % 20 - 10) / 5.0f, z * 5 + (float)(rand() % 20 - 10) / 5.0f) + glm::vec3(0.3f, -43.0f, 0.0f));
                newDeb.setColor(glm::vec3(0.02f, 0.02f, 0.02f) * glm::vec3(x, y, z));
                mDebuggers.push_back(newDeb);
            }
        }
    }
}

void RenderingSystem::handleMessage(const RotateGfxEntityMessage& received)
{
    size_t id = received.id;
    float pitch = received.pitch;
    float yaw = received.yaw;

    if(id == mCameraEntity)
    {
        mRenderer.getCamera().setPitchYaw(pitch, yaw);
    }
}

void RenderingSystem::handleMessage(const MoveGfxEntityMessage& received)
{
    size_t id = received.id;
    const glm::vec3& position = received.position;

    if(id == mCameraEntity)
    {
        mRenderer.getCamera().setPosition(position);
    }
}

void RenderingSystem::handleMessage(const ClientAttachedToEntityMessage& received)
{
    mCameraEntity = received.entityId;
}

void RenderingSystem::handleMessage(const WindowResizeMessage& received)
{
	uint32_t width = received.width;
	uint32_t height = received.height;

    mRenderer.setViewSize({width, height});
}

void RenderingSystem::handleMessage(const RenderModeMessage& received)
{
    if(received.type == DISABLE_ALL)
        mRenderer.setEnabled(false);
    else if(received.type == ENABLE_ALL)
        mRenderer.setEnabled(true);
    else if(received.type == TOGGLE_MODE_ALL)
    {
        PolygonMode current = mRenderer.getRenderMode().getPolygonMode();
        mRenderer.getRenderMode().setPolygonMode((int32_t)current + 1 > (int32_t)PolygonMode::POINT ? PolygonMode::FILL : (PolygonMode)((int32_t)current + 1));
    }
    else if(received.type == DISABLE_DEBUG)
        mRenderer.setEnabled(RenderModule::DEBUG, false);
    else if(received.type == ENABLE_DEBUG)
        mRenderer.setEnabled(RenderModule::DEBUG, true);
    else if(received.type == TOGGLE_MODE_DEBUG)
    {
        mRenderer.enableRenderMode(RenderModule::DEBUG, true);

        PolygonMode current = mRenderer.getRenderMode(RenderModule::DEBUG).getPolygonMode();
        mRenderer.getRenderMode(RenderModule::DEBUG).setPolygonMode((int32_t)current + 1 > (int32_t)PolygonMode::POINT ? PolygonMode::FILL : (PolygonMode)((int32_t)current + 1));
    }
    else if(received.type == DISABLE_MODEL)
        mRenderer.setEnabled(RenderModule::MODEL, false);
    else if(received.type == ENABLE_MODEL)
        mRenderer.setEnabled(RenderModule::MODEL, true);
    else if(received.type == TOGGLE_MODE_MODEL)
    {
        mRenderer.enableRenderMode(RenderModule::MODEL, true);

        PolygonMode current = mRenderer.getRenderMode(RenderModule::MODEL).getPolygonMode();
        mRenderer.getRenderMode(RenderModule::MODEL).setPolygonMode((int32_t)current + 1 > (int32_t)PolygonMode::POINT ? PolygonMode::FILL : (PolygonMode)((int32_t)current + 1));
    }
}

void RenderingSystem::render()
{
    for(auto& debbie : mDebuggers)
    {
        debbie.setColor({(float)(rand() % 256) / 256.0f,(float)(rand() % 256) / 256.0f, (float)(rand() % 256) / 256.0f});
        mRenderer.queue(debbie);
    }

    mRenderer.queue(mCubeModelRenderable);
    mRenderer.queue(mTetraModelRenderable);

    mRenderer.render();
}
