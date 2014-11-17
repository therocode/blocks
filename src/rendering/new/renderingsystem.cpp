#include "renderingsystem.hpp"
#include "debugrenderer.hpp"

RenderingSystem::RenderingSystem(fea::MessageBus& bus) :
    mBus(bus),
    mRenderer(mGLContext)

{
    subscribe(bus, *this);
    mRenderer.addModule(std::unique_ptr<DebugRenderer>(new DebugRenderer()));

    for(uint32_t x = 0; x < 50; x++)
    {
        for(uint32_t y = 0; y < 50; y++)
        {
            for(uint32_t z = 0; z < 50; z++)
            {
                DebugRenderable newDeb;
                newDeb.setPosition(glm::vec3(x * 5, y * 5, z * 5) + glm::vec3(0.3f, -43.0f, 0.0f));
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

void RenderingSystem::render()
{
    for(const auto& debbie : mDebuggers)
        mRenderer.queue(debbie);

    mRenderer.render();
}
