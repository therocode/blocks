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

    for(uint32_t x = 0; x < 25; x++)
    {
        for(uint32_t y = 0; y < 25; y++)
        {
            for(uint32_t z = 0; z < 25; z++)
            {
                DebugRenderable newDeb;
                newDeb.setPosition(glm::vec3(x * 5 + (float)(rand() % 20 - 10) / 5.0f, y * 5 + (float)(rand() % 20 - 10) / 5.0f, z * 5 + (float)(rand() % 20 - 10) / 5.0f) + glm::vec3(0.3f, -43.0f, 0.0f));
                newDeb.setColor(glm::vec3(0.02f, 0.02f, 0.02f) * glm::vec3(x, y, z));
				newDeb.setPitch(0.0f);
				newDeb.setYaw(0.0f);
                mDebuggers.push_back(newDeb);
            }
        }
    }
}

void RenderingSystem::handleMessage(const AddGfxEntityMessage& received)
{
    ModelRenderable newModel;
    
    newModel.setModel(**mModels.begin());
    newModel.setPosition(received.position);
    newModel.setColor({(float)(rand() % 256) / 256.0f,(float)(rand() % 256) / 256.0f, (float)(rand() % 256) / 256.0f});

    mModelRenderables.emplace(received.id, newModel);
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

    mModelRenderables.at(received.id).setPosition(position);
}

void RenderingSystem::handleMessage(const RemoveGfxEntityMessage& received)
{
    mModelRenderables.erase(received.id);
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

void RenderingSystem::handleMessage(const ModelDeliverMessage& received)
{
    std::unique_ptr<Model> newModel = std::unique_ptr<Model>(new Model());
    newModel->addVertexArray(Model::POSITIONS, received.model->positions);

    int32_t meshNumber = 0;
    for(const auto& indices : received.model->indices)
    {
        std::unique_ptr<Mesh> mesh = std::unique_ptr<Mesh>(new Mesh(indices));
        newModel->addMesh(meshNumber, std::move(mesh));
        meshNumber++;
    }

    mModels.push_back(std::move(newModel));
}

void RenderingSystem::render()
{
    for(auto& debbie : mDebuggers)
    {
        debbie.setColor({(float)(rand() % 256) / 256.0f,(float)(rand() % 256) / 256.0f, (float)(rand() % 256) / 256.0f});
        debbie.setPitch(debbie.getPitch()+0.2f);
        debbie.setYaw(debbie.getYaw()+0.1f);
		mRenderer.queue(debbie);
    }

    for(auto& moddie : mModelRenderables)
    {
        mRenderer.queue(moddie.second);
    }

    mRenderer.render();
}
