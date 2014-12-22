#include "renderingsystem.hpp"
#include "modelrenderer.hpp"
#include "voxelchunkrenderer.hpp"
#include "extrarenderer.hpp"
#include "debugrenderer.hpp"
#include "../resources/rawmodel.hpp"
#include "../resources/shadersource.hpp"
#include "../resources/shaderdefinition.hpp"
#include "shaderattribute.hpp"
#include "opengl.hpp"
#include "../lognames.hpp"
#include "../application/applicationmessages.hpp"

RenderingSystem::RenderingSystem(fea::MessageBus& bus, const glm::uvec2& viewSize) :
    mBus(bus),
    mRenderer(mGLContext, viewSize),
    mIsFacing(false)

{
    subscribe(bus, *this);
    mRenderer.addModule(RenderModule::MODEL, std::unique_ptr<ModelRenderer>(new ModelRenderer()));
    mRenderer.addModule(RenderModule::VOXEL, std::unique_ptr<VoxelChunkRenderer>(new VoxelChunkRenderer()));
    mRenderer.addModule(RenderModule::EXTRA, std::unique_ptr<ExtraRenderer>(new ExtraRenderer()));
    mRenderer.addModule(RenderModule::DEBUG, std::unique_ptr<DebugRenderer>(new DebugRenderer()));

    for(uint32_t x = 0; x < 25; x++)
    {
        for(uint32_t y = 0; y < 25; y++)
        {
            for(uint32_t z = 0; z < 25; z++)
            {
                DebugRenderable newDeb(DebugRenderable::CUBE);
                newDeb.setPosition(glm::vec3(x * 5 + (float)(rand() % 20 - 10) / 5.0f, y * 5 + (float)(rand() % 20 - 10) / 5.0f, z * 5 + (float)(rand() % 20 - 10) / 5.0f) + glm::vec3(0.3f, -43.0f, 0.0f));
                newDeb.setColor(glm::vec3(0.02f, 0.02f, 0.02f) * glm::vec3(x, y, z));
				newDeb.setPitch(0.0f);
				newDeb.setYaw(0.0f);
                //mDebuggers.push_back(newDeb);
            }
        }
    }

    for(uint32_t i = 0; i < 100; i++)
    {
        glm::vec3 pos(-24 + i, 41.0f, -30.0f);
        DebugRenderable newDeb2(DebugRenderable::LINE);
        newDeb2.setLinePoints(pos, pos + glm::vec3(0.0f,  3.0f, 0.0f));
        newDeb2.setLineColors({(float)(rand() % 256) / 256.0f,(float)(rand() % 256) / 256.0f, (float)(rand() % 256) / 256.0f}, {(float)(rand() % 256) / 256.0f,(float)(rand() % 256) / 256.0f, (float)(rand() % 256) / 256.0f});
        mDebuggers.push_back(newDeb2);
    }

    GLint maxSize;
    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxSize);

    if(maxSize < 2048)
        mBus.send(LogMessage{"Only supporting " + std::to_string(maxSize) + " texture layers.", gRenderingName, LogLevel::WARN});
}

void RenderingSystem::handleMessage(const AddGfxEntityMessage& received)
{
    std::cout << " asked to add entity " << received.id << "\n";

    if(received.id != mCameraEntity)
    {
        ModelRenderable newModel;

        newModel.setModel(**(mModels.begin() + rand() % mModels.size()));
        newModel.setTexture(*mTextureArrays.at(0), rand() % 2);
        newModel.setPosition(received.position);
        newModel.setColor({(float)(rand() % 256) / 256.0f,(float)(rand() % 256) / 256.0f, (float)(rand() % 256) / 256.0f});

        mModelRenderables.emplace(received.id, newModel);
    }
}

void RenderingSystem::handleMessage(const RotateGfxEntityMessage& received)
{
    size_t id = received.id;
    float pitch = received.pitch;
    float yaw = received.yaw;

    if(received.id != mCameraEntity)
    {
        mModelRenderables.at(received.id).setPitch(pitch);
        mModelRenderables.at(received.id).setYaw(yaw);
    }
    else
    {
        mRenderer.getCamera().setPitchYaw(pitch, yaw);
    }
}

void RenderingSystem::handleMessage(const MoveGfxEntityMessage& received)
{
    size_t id = received.id;
    const glm::vec3& position = received.position;

    //std::cout << " asked to move entity " << id << "\n";

    if(received.id != mCameraEntity)
    {
        mModelRenderables.at(received.id).setPosition(position);
    }
    else
    {
        mRenderer.getCamera().setPosition(position);
    }
}

void RenderingSystem::handleMessage(const RemoveGfxEntityMessage& received)
{
    if(received.id != mCameraEntity)
    {
        mModelRenderables.erase(received.id);
    }
}

void RenderingSystem::handleMessage(const ClientAttachedToEntityMessage& received)
{
    std::cout << "got to know that camera entity is " << received.entityId << "\n";
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
    else if(received.type == DISABLE_VOXEL)
        mRenderer.setEnabled(RenderModule::VOXEL, false);
    else if(received.type == ENABLE_VOXEL)
        mRenderer.setEnabled(RenderModule::VOXEL, true);
    else if(received.type == TOGGLE_MODE_VOXEL)
    {
        mRenderer.enableRenderMode(RenderModule::VOXEL, true);

        PolygonMode current = mRenderer.getRenderMode(RenderModule::VOXEL).getPolygonMode();
        mRenderer.getRenderMode(RenderModule::VOXEL).setPolygonMode((int32_t)current + 1 > (int32_t)PolygonMode::POINT ? PolygonMode::FILL : (PolygonMode)((int32_t)current + 1));
    }
}

void RenderingSystem::handleMessage(const ResourceDeliverMessage<RawModel>& received)
{
    std::unique_ptr<Model> newModel = std::unique_ptr<Model>(new Model());
    newModel->addVertexArray(Model::POSITIONS, received.resource->positions);
    newModel->addVertexArray(Model::NORMALS, received.resource->normals);
    newModel->addVertexArray(Model::TEXCOORDS, received.resource->texCoords);

    int32_t meshNumber = 0;
    for(const auto& indices : received.resource->indices)
    {
        std::unique_ptr<Mesh> mesh = std::unique_ptr<Mesh>(new Mesh(indices));
        newModel->addMesh(meshNumber, std::move(mesh));
        meshNumber++;
    }

    mModels.push_back(std::move(newModel));
}

void RenderingSystem::handleMessage(const ResourceDeliverMessage<ShaderSource>& received) 
{
    if(received.resource->type == ShaderSource::VERTEX)
    {
        mVertexSources.emplace(received.resource->name, received.resource->source);
    }
    else if(received.resource->type == ShaderSource::FRAGMENT)
    {
        mFragmentSources.emplace(received.resource->name, received.resource->source);
    }
}

void RenderingSystem::handleMessage(const ResourceDeliverMessage<ShaderDefinition>& received) 
{
    std::unique_ptr<Shader> shader = std::unique_ptr<Shader>(new Shader());

    shader->setSource(mVertexSources.at(received.resource->vertexShader), mFragmentSources.at(received.resource->fragmentShader));

    shader->compile({
            {"POSITION", ShaderAttribute::POSITION},
            {"NORMAL", ShaderAttribute::NORMAL},
            {"TEXCOORD", ShaderAttribute::TEXCOORD},
            {"COLOR", ShaderAttribute::COLOR},
            {"TEXTUREINDEX", ShaderAttribute::TEXTUREINDEX},
            {"MODELMATRIX1", ShaderAttribute::MODELMATRIX1},
            {"MODELMATRIX2", ShaderAttribute::MODELMATRIX2},
            {"MODELMATRIX3", ShaderAttribute::MODELMATRIX3},
            {"MODELMATRIX4", ShaderAttribute::MODELMATRIX4},
            {"NORMALMATRIX1", ShaderAttribute::NORMALMATRIX1},
            {"NORMALMATRIX2", ShaderAttribute::NORMALMATRIX2},
            {"NORMALMATRIX3", ShaderAttribute::NORMALMATRIX3},
            {"NORMALMATRIX4", ShaderAttribute::NORMALMATRIX4}
            });

    mShaders.emplace(received.id, std::move(shader));
}

void RenderingSystem::handleMessage(const ResourceDeliverMessage<TextureArray>& received)
{
    mTextureArrays.push_back(received.resource);
}

void RenderingSystem::handleMessage(const UpdateChunkVboMessage& received)
{
    const ChunkCoord& mainChunkCoord = received.mainChunkCoord;
    Chunk* mainChunk = received.main;
    Chunk* topChunk = received.top;
    Chunk* bottomChunk = received.bottom;
    Chunk* frontChunk = received.front;
    Chunk* backChunk = received.back;
    Chunk* leftChunk = received.left;
    Chunk* rightChunk = received.right;

    mChunkModels[mainChunkCoord] = mChunkModelCreator.generateChunkModel(mainChunkCoord, mainChunk, topChunk, bottomChunk, frontChunk, backChunk, leftChunk, rightChunk);
}

void RenderingSystem::handleMessage(const ChunkDeletedMessage& received)
{
    mChunkModels.erase(received.coordinate);
}

void RenderingSystem::handleMessage(const FacingBlockMessage& received)
{
    mIsFacing = received.inRange;
    mFacingBlock = received.block;
}

void RenderingSystem::render()
{
    for(auto& debbie : mDebuggers)
    {
        //debbie.setColor({(float)(rand() % 256) / 256.0f,(float)(rand() % 256) / 256.0f, (float)(rand() % 256) / 256.0f});
        //debbie.setPitch(debbie.getPitch()+0.2f);
        //debbie.setYaw(debbie.getYaw()+0.1f);
		mRenderer.queue(debbie);
    }

    for(auto& moddie : mModelRenderables)
    {
        mRenderer.queue(moddie.second);
    }

    for(auto& voxie : mChunkModels)
    {
        VoxelChunkRenderable renderable;
        renderable.setTexture(*mTextureArrays.at(1));
        renderable.setModel(voxie.second);
        mRenderer.queue(renderable);
    }

    if(mIsFacing)
    {
        ExtraRenderable extra;
        extra.setPosition((glm::vec3)mFacingBlock + glm::vec3(0.5f, 0.5f, 0.5f));
        extra.setColor({1.0f, 1.0f, 1.0f});
        mRenderer.queue(extra);
    }

    mRenderer.render(*mShaders.begin()->second);
}
