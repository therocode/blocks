#pragma once
#include "renderable.hpp"
#include "../utilities/glm.hpp"

class Model;
class TextureArray;

class VoxelChunkRenderable : public Renderable
{
    public:
        VoxelChunkRenderable();
        virtual std::type_index getType() const;
        void setModel(const Model& model);
        const Model* findModel() const;
        void setTexture(const TextureArray& texture, uint32_t index);
        const TextureArray* findTextureArray() const;
        uint32_t getTextureIndex() const;
    private:
        const Model* mModel;
        const TextureArray* mTextureArray;
        uint32_t mTextureIndex;
};
