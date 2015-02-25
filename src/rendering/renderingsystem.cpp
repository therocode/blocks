#include "renderingsystem.hpp"
#include "baseshader.hpp"
#include "animationshader.hpp"
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
#include "shaderassembler.hpp"

RenderingSystem::RenderingSystem(fea::MessageBus& bus, const glm::uvec2& viewSize) :
    mBus(bus),
    mRenderer(mGLContext, viewSize),
    mIsFacing(false),
    mMeshWorkerPool(3)
{
    subscribe(bus, *this);
    mRenderer.addModule(RenderModule::MODEL, std::unique_ptr<ModelRenderer>(new ModelRenderer()));
    mRenderer.addModule(RenderModule::VOXEL, std::unique_ptr<VoxelChunkRenderer>(new VoxelChunkRenderer()));
    mRenderer.addModule(RenderModule::EXTRA, std::unique_ptr<ExtraRenderer>(new ExtraRenderer()));
    mRenderer.addModule(RenderModule::DEBUG, std::unique_ptr<DebugRenderer>(new DebugRenderer()));

    GLint maxSize;
    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxSize);

    if(maxSize < 2048)
        mBus.send(LogMessage{"Only supporting " + std::to_string(maxSize) + " texture layers.", gRenderingName, LogLevel::WARN});
}

void RenderingSystem::handleMessage(const AddGfxEntityMessage& received)
{
    FEA_ASSERT(mGfxEntityDefinitions.count(received.type) != 0, "Invalid GfxEntity type received");
    const GfxEntityDefinition& definition = *mGfxEntityDefinitions.at(received.type);
    FEA_ASSERT(mModels.count(definition.modelId) != 0, "Invalid model given");
    const Model& model = *mModels.at(definition.modelId);
    FEA_ASSERT(mTextureDefinitions.count(definition.textureId) != 0, "Invalid texture given");
    const TextureDefinition& textureDefinition = *mTextureDefinitions.at(definition.textureId);
    FEA_ASSERT(mTextureArrays.count(textureDefinition.textureArrayId) != 0, "Invalid texture array given");
    const TextureArray& textureArray = *mTextureArrays.at(textureDefinition.textureArrayId);
    size_t newId = mEntityIds.next();
    ModelRenderable newModel;

    newModel.setModel(model);
    newModel.setTexture(textureArray, textureDefinition.index);
    newModel.setPosition(received.position);
    newModel.setColor({(float)(rand() % 256) / 256.0f,(float)(rand() % 256) / 256.0f, (float)(rand() % 256) / 256.0f});
    newModel.setFrameOffset(rand() % 100);
    newModel.setAnimation("");

    mModelRenderables.emplace(newId, newModel);

    received.returnedId = newId;
}

void RenderingSystem::handleMessage(const OrientateGfxEntityMessage& received)
{
    size_t id = received.id;
    glm::quat orientation = received.orientation;

    if(received.id != mCameraEntity)
    {
        auto iterator = mModelRenderables.find(id);
        if(iterator != mModelRenderables.end())
        {
            iterator->second.setOrientation(orientation);
        }
    }
    else
    {
        mRenderer.getCamera().setOrientation(orientation);
    }
}

void RenderingSystem::handleMessage(const MoveGfxEntityMessage& received)
{
    size_t id = received.id;
    const glm::vec3& position = received.position;

    if(received.id != mCameraEntity)
    {
        auto iterator = mModelRenderables.find(id);
        if(iterator != mModelRenderables.end())
        {
            iterator->second.setPosition(position);
        }
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
        mEntityIds.free(received.id);
    }
}

void RenderingSystem::handleMessage(const AttachedToGfxEntityMessage& received)
{
    mCameraEntity = received.id;

    mModelRenderables.erase(mCameraEntity);
}

void RenderingSystem::handleMessage(const AnimateGfxEntityMessage& received)
{
    if(received.id != mCameraEntity)
    {
        FEA_ASSERT(mModelRenderables.count(received.id) != 0, "Trying to animate a non existing entity");
        auto& model = mModelRenderables.at(received.id);
        model.setAnimation(received.animationName);
        model.setFrameOffset(0.0f);
    }
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
    newModel->addVertexArray(ModelAttribute::POSITIONS, received.resource->positions);
    newModel->addVertexArray(ModelAttribute::NORMALS, received.resource->normals);
    newModel->addVertexArray(ModelAttribute::TEXCOORDS, received.resource->texCoords);
    newModel->addBlendArray(ModelAttribute::BLENDWEIGHTS, received.resource->blendWeights);
    newModel->addBlendArray(ModelAttribute::BLENDINDICES, received.resource->blendIndices);

    int32_t meshNumber = 0;
    for(const auto& indices : received.resource->indices)
    {
        std::unique_ptr<Mesh> mesh = std::unique_ptr<Mesh>(new Mesh(indices));
        newModel->addMesh(meshNumber, std::move(mesh));
        meshNumber++;
    }

    for(const auto anim : received.resource->animations)
    {
        std::unique_ptr<Animation> animation = std::unique_ptr<Animation>(new Animation());

        animation->framerate = anim.framerate;
        animation->frameAmount = anim.frameAmount;
        animation->rotations = std::move(anim.rotations);
        animation->translations = std::move(anim.translations);

        newModel->addAnimation(anim.name, std::move(animation));
    }

    newModel->setJointStructure(received.resource->jointStructure);

    mModels.emplace(received.id, std::move(newModel));
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
    std::vector<std::string> vertexModules;

    for(const auto& moduleName : received.resource->vertexModules)
        vertexModules.push_back(mVertexSources.at(moduleName));

    std::vector<std::string> fragmentModules;

    for(const auto& moduleName : received.resource->fragmentModules)
        fragmentModules.push_back(mFragmentSources.at(moduleName));

    ShaderAssembler assembler;

    std::unique_ptr<Shader> baseShader = std::unique_ptr<Shader>(new Shader());
    baseShader->setSource(assembler.assemble(BaseShader::vertexSource, vertexModules), assembler.assemble(BaseShader::fragmentSource, fragmentModules));

    baseShader->compile({
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

    mBaseShaders.emplace(received.id, std::move(baseShader));

    std::unique_ptr<Shader> animationShader = std::unique_ptr<Shader>(new Shader());
    animationShader->setSource(assembler.assemble(AnimationShader::vertexSource, vertexModules), assembler.assemble(AnimationShader::fragmentSource, fragmentModules));

    animationShader->compile({
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
            {"NORMALMATRIX4", ShaderAttribute::NORMALMATRIX4},
            {"ANIMATIONDATA", ShaderAttribute::ANIMATIONDATA},
            {"BLENDINDICES", ShaderAttribute::BLENDINDICES},
            {"BLENDWEIGHTS", ShaderAttribute::BLENDWEIGHTS}
            });

    mAnimationShaders.emplace(received.id, std::move(animationShader));
}

void RenderingSystem::handleMessage(const ResourceDeliverMessage<TextureArray>& received)
{
    mTextureArrays.emplace(received.id, received.resource);
}

void RenderingSystem::handleMessage(const ResourceDeliverMessage<TextureDefinition>& received)
{
    mTextureDefinitions.emplace(received.id, received.resource);
}

void RenderingSystem::handleMessage(const ResourceDeliverMessage<GfxEntityDefinition>& received)
{
    mGfxEntityDefinitions.emplace(received.id, received.resource);
}

void RenderingSystem::handleMessage(const UpdateChunkVboMessage& received)
{
    mActiveChunks.emplace(received.mainChunkCoord);
    const ChunkCoord& mainChunkCoord = received.mainChunkCoord;
    std::shared_ptr<Chunk> top= received.top == nullptr ? std::shared_ptr<Chunk>() : std::shared_ptr<Chunk>(new Chunk(*received.top));
    std::shared_ptr<Chunk> bottom= received.bottom == nullptr ? std::shared_ptr<Chunk>() : std::shared_ptr<Chunk>(new Chunk(*received.bottom));
    std::shared_ptr<Chunk> front= received.front == nullptr ? std::shared_ptr<Chunk>() : std::shared_ptr<Chunk>(new Chunk(*received.front));
    std::shared_ptr<Chunk> back= received.back == nullptr ? std::shared_ptr<Chunk>() : std::shared_ptr<Chunk>(new Chunk(*received.back));
    std::shared_ptr<Chunk> left= received.left == nullptr ? std::shared_ptr<Chunk>() : std::shared_ptr<Chunk>(new Chunk(*received.left));
    std::shared_ptr<Chunk> right= received.right == nullptr ? std::shared_ptr<Chunk>() : std::shared_ptr<Chunk>(new Chunk(*received.right));

    //add chunk to load to other thread
    auto bound = std::bind(&RenderingSystem::generateChunkModel, this, mainChunkCoord, *received.main, top, bottom, front, back, left, right);

    mChunkModelsToFinish.push_back(mMeshWorkerPool.enqueue(bound, 0));
}

void RenderingSystem::handleMessage(const ChunkDeletedMessage& received)
{
    mChunkModels.erase(received.coordinate);
    mActiveChunks.erase(received.coordinate);
}

void RenderingSystem::handleMessage(const FacingBlockMessage& received)
{
    mIsFacing = received.inRange;
    mFacingBlock = received.block;
}

void RenderingSystem::render()
{
    fetchDoneChunkMeshes();
    mRenderer.clear();

    for(auto& voxie : mChunkModels)
    {
        VoxelChunkRenderable renderable;
        renderable.setTexture(*mTextureArrays.at(1));
        renderable.setModel(voxie.second.second);
        renderable.setCoordinate(voxie.first);
        if(voxie.second.first)
        {
            renderable.updated();
            voxie.second.first = false;
        }
        mRenderer.queue(renderable);
    }

    if(mIsFacing)
    {
        ExtraRenderable extra;
        extra.setPosition((glm::vec3)mFacingBlock + glm::vec3(0.5f, 0.5f, 0.5f));
        extra.setColor({1.0f, 1.0f, 1.0f});
        mRenderer.queue(extra);
    }

    mRenderer.render(*mBaseShaders.at(0));

    for(auto& moddie : mModelRenderables)
    {
        moddie.second.setFrameOffset(moddie.second.getFrameOffset() + 1.0f);
        mRenderer.queue(moddie.second);
    }

    mRenderer.render(*mAnimationShaders.at(0));
}

ChunkModelDelivery RenderingSystem::generateChunkModel(ChunkCoord coordinate, Chunk main, std::shared_ptr<Chunk> top, std::shared_ptr<Chunk> bottom, std::shared_ptr<Chunk> front, std::shared_ptr<Chunk> back, std::shared_ptr<Chunk> left, std::shared_ptr<Chunk> right)
{
    ChunkModelCreator creator;
    return creator.generateChunkModel(coordinate, main, top.get(), bottom.get(), front.get(), back.get(), left.get(), right.get());
}

void RenderingSystem::fetchDoneChunkMeshes()
{
    for(auto iter = mChunkModelsToFinish.begin(); iter != mChunkModelsToFinish.end();)
    {
        if(iter->wait_for(std::chrono::seconds(0)) == std::future_status::ready)
        {
            ChunkModelDelivery finished = std::move(iter->get());

            if(mActiveChunks.count(finished.coordinate) != 0)
                mChunkModels[finished.coordinate] = {true, std::move(finished.model)};

            iter = mChunkModelsToFinish.erase(iter);
        }
        else
        {
            iter++;
        }
    }
}
