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

    GLint maxSize;
    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxSize);

    if(maxSize < 2048)
        mBus.send(LogMessage{"Only supporting " + std::to_string(maxSize) + " texture layers.", gRenderingName, LogLevel::WARN});
}

void RenderingSystem::handleMessage(const AddGfxEntityMessage& received)
{
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

void RenderingSystem::handleMessage(const OrientateGfxEntityMessage& received)
{
    size_t id = received.id;
    float pitch = glm::pitch(received.orientation);
    float yaw = glm::yaw(received.orientation);

    if(received.id != mCameraEntity)
    {
        auto iterator = mModelRenderables.find(id);
        if(iterator != mModelRenderables.end())
        {
            iterator->second.setPitch(pitch);
            iterator->second.setYaw(yaw);
        }
    }
    else
    {
        mRenderer.getCamera().setOrientation(received.orientation);
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
    newModel->addVertexArray(Model::BLENDWEIGHTS, received.resource->blendWeights);
    newModel->addVertexArray(Model::BLENDINDICES, received.resource->blendIndices);

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

// Note that this animates all attributes (position, normal, tangent, bitangent)
// for expository purposes, even though this demo does not use all of them for rendering.
//  void animateiqm(float curframe)
//  {
//      if(!numframes) return;
//  
//      int frame1 = (int)floor(curframe),
//          frame2 = frame1 + 1;
//      float frameoffset = curframe - frame1;
//      frame1 %= numframes;
//      frame2 %= numframes;
//      Matrix3x4 *mat1 = &frames[frame1 * numjoints],
//                *mat2 = &frames[frame2 * numjoints];
//      // Interpolate matrixes between the two closest frames and concatenate with parent matrix if necessary.
//      // Concatenate the result with the inverse of the base pose.
//      // You would normally do animation blending and inter-frame blending here in a 3D engine.
//      for(int i = 0; i < numjoints; i++)
//      {   
//          Matrix3x4 mat = mat1[i]*(1 - frameoffset) + mat2[i]*frameoffset;
//          if(joints[i].parent >= 0) outframe[i] = outframe[joints[i].parent] * mat;
//          else outframe[i] = mat;
//      }   
//      // The actual vertex generation based on the matrixes follows...
//      const Vec3 *srcpos = (const Vec3 *)inposition, *srcnorm = (const Vec3 *)innormal;
//      const Vec4 *srctan = (const Vec4 *)intangent; 
//      Vec3 *dstpos = (Vec3 *)outposition, *dstnorm = (Vec3 *)outnormal, *dsttan = (Vec3 *)outtangent, *dstbitan = (Vec3 *)outbitangent; 
//      const uchar *index = inblendindex, *weight = inblendweight;
//      for(int i = 0; i < numverts; i++)
//      {
//          // Blend matrixes for this vertex according to its blend weights. 
//          // the first index/weight is always present, and the weights are
//          // guaranteed to add up to 255. So if only the first weight is
//          // presented, you could optimize this case by skipping any weight
//          // multiplies and intermediate storage of a blended matrix. 
//          // There are only at most 4 weights per vertex, and they are in 
//          // sorted order from highest weight to lowest weight. Weights with 
//          // 0 values, which are always at the end, are unused.
//          Matrix3x4 mat = outframe[index[0]] * (weight[0]/255.0f);
//          for(int j = 1; j < 4 && weight[j]; j++)
//              mat += outframe[index[j]] * (weight[j]/255.0f);
//  
//          // Transform attributes by the blended matrix.
//          // Position uses the full 3x4 transformation matrix.
//          // Normals and tangents only use the 3x3 rotation part 
//          // of the transformation matrix.
//          *dstpos = mat.transform(*srcpos);
//  
//          // Note that if the matrix includes non-uniform scaling, normal vectors
//          // must be transformed by the inverse-transpose of the matrix to have the
//          // correct relative scale. Note that invert(mat) = adjoint(mat)/determinant(mat),
//          // and since the absolute scale is not important for a vector that will later
//          // be renormalized, the adjoint-transpose matrix will work fine, which can be
//          // cheaply generated by 3 cross-products.
//          //
//          // If you don't need to use joint scaling in your models, you can simply use the
//          // upper 3x3 part of the position matrix instead of the adjoint-transpose shown 
//          // here.
//          Matrix3x3 matnorm(mat.b.cross3(mat.c), mat.c.cross3(mat.a), mat.a.cross3(mat.b));
//  
//          *dstnorm = matnorm.transform(*srcnorm);
//          // Note that input tangent data has 4 coordinates, 
//          // so only transform the first 3 as the tangent vector.
//          *dsttan = matnorm.transform(Vec3(*srctan));
//          // Note that bitangent = cross(normal, tangent) * sign, 
//          // where the sign is stored in the 4th coordinate of the input tangent data.
//          *dstbitan = dstnorm->cross(*dsttan) * srctan->w;
//  
//          srcpos++;
//          srcnorm++;
//          srctan++;
//          dstpos++;
//          dstnorm++;
//          dsttan++;
//          dstbitan++;
//  
//          index += 4;
//          weight += 4;
//      }
//  }

void RenderingSystem::render()
{
    mRenderer.clear();

    for(auto& debbie : mDebuggers)
    {
        //debbie.setColor({(float)(rand() % 256) / 256.0f,(float)(rand() % 256) / 256.0f, (float)(rand() % 256) / 256.0f});
        //debbie.setPitch(debbie.getPitch()+0.2f);
        //debbie.setYaw(debbie.getYaw()+0.1f);
		mRenderer.queue(debbie);
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

    mRenderer.render(*mBaseShaders.at(0));

    for(auto& moddie : mModelRenderables)
    {
        mRenderer.queue(moddie.second);
    }


    mRenderer.render(*mAnimationShaders.at(0));
}
